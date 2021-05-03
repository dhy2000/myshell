#include "exe.h"

#include <string.h>
#include <unistd.h>

int execute(const struct CommandRedirect *command) {
    // returns the pid of child process
    // check exit
    if (strcmp(command->cmd, "exit") == 0) {
        _exit(0);
    }
    // create subprocess
    int pid = fork();
    if (pid < 0) {
        write(STDERR_FILENO, "error: fork error\n", 18);
        return -1;
    } else if (pid == 0) {
        // child process
        if (command->unused_pipe) {
            close(command->unused_pipe);
        }
        if (command->stdin_des != STDIN_FILENO) {
            dup2(command->stdin_des, STDIN_FILENO);
        }
        if (command->stdout_des != STDOUT_FILENO) {
            dup2(command->stdout_des, STDOUT_FILENO);
        }
        int r = execve(command->cmd, NULL, NULL);
        if (r < 0) {
            write(STDERR_FILENO, "error: command invalid\n", 23);
            _exit(-1);
        }
        return 0; // never reach
    } else {
        // father process
        return pid;
    }
}