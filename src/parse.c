#include "parse.h"

#include "exe.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <string.h>

/*
 * cmd 
 * cmd < file
 * cmd > file
 * cmd >> file
 * cmd | cmd
 */

#define MAX_PIPE_STAGE 8

struct CommandRedirect command[MAX_PIPE_STAGE];

int parse(char *input) {
    int icmd = 0;
    command[icmd].cmd = strtok(input, " ");
    command[icmd].stdin_des = STDIN_FILENO;
    command[icmd].stdout_des = STDOUT_FILENO;

    if (command[icmd].cmd == NULL) {
        write(STDERR_FILENO, "error: null command\n", 20);
        return -1;
    }
    while (1) {
        char *tok = strtok(NULL, " ");
        if (tok == NULL) {
            break;
        }
        if (strcmp(tok, "<") == 0) {
            tok = strtok(NULL, " ");
            if (tok == NULL) {
                write(STDERR_FILENO, "error: unexpected EOL after '<'\n", 32);
                return -1;
            }
            command[icmd].stdin_des = open(tok, O_RDONLY);
            if (command[icmd].stdin_des == -1) {
                write(STDERR_FILENO, "error: cannot open file ", 24);
                write(STDERR_FILENO, tok, strlen(tok));
                write(STDERR_FILENO, "\n", 1);
                return -1;
            }
        } else if (strcmp(tok, ">") == 0) {
            tok = strtok(NULL, " ");
            if (tok == NULL) {
                write(STDERR_FILENO, "error: unexpected EOL after '>'\n", 32);
                return -1;
            }
            command[icmd].stdout_des = open(tok, O_WRONLY | O_CREAT, 0644);
            if (command[icmd].stdout_des == -1) {
                write(STDERR_FILENO, "error: cannot open file ", 24);
                write(STDERR_FILENO, tok, strlen(tok));
                write(STDERR_FILENO, "\n", 1);
                return -1;
            }
        } else if (strcmp(tok, ">>") == 0) {
            tok = strtok(NULL, " ");
            if (tok == NULL) {
                write(STDERR_FILENO, "error: unexpected EOL after '>>'\n", 33);
                return -1;
            }
            command[icmd].stdout_des = open(tok, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (command[icmd].stdout_des == -1) {
                write(STDERR_FILENO, "error: cannot open file ", 24);
                write(STDERR_FILENO, tok, strlen(tok));
                write(STDERR_FILENO, "\n", 1);
                return -1;
            }
        } else if (strcmp(tok, "|") == 0) {
            icmd++;
            command[icmd].stdin_des = STDIN_FILENO;
            command[icmd].stdout_des = STDOUT_FILENO;
            if (icmd >= MAX_PIPE_STAGE) {
                write(STDERR_FILENO, "error: too much pipe stage\n", 27);
                return -1;
            }
            continue;
        } else {
            command[icmd].cmd = tok;
        }
    }
    // create pipe
    if (icmd > 0) {
        int i;
        for (i = 0; i < icmd; i++) {
            int fildes[2];
            int p = pipe(fildes);
            if (p < 0) {
                write(STDERR_FILENO, "error: failed to create pipe\n", 29);
                return -1;
            }
            command[i].stdout_des = fildes[1];
            command[i].unused_pipe = fildes[0];
            command[i + 1].stdin_des = fildes[0];
            command[i + 1].unused_pipe = fildes[1];
        }
    }
    // run
    int i;
    for (i = 0; i <= icmd; i++) {
        command[i].pid = execute(&command[i]);
    }
    // wait end and close file
    for (i = 0; i <= icmd; i++) {
        wait(&command[i].pid);
        if (command[i].stdin_des >= 3) {
            close(command[i].stdin_des);
        }
        if (command[i].stdout_des >= 3) {
            close(command[i].stdout_des);
        }
    }
}
