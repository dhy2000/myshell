#include <stdio.h>
#include <unistd.h>

#include "parse.h"

#define MAX_LINE_LENGTH 1024

char line_input[MAX_LINE_LENGTH];

int main(int argc, char *argv[]) {

    write(STDOUT_FILENO, "$ ", 2);
    while (fgets(line_input, MAX_LINE_LENGTH, stdin) != NULL) {
        // input check
        int l = 0;
        for (; l < MAX_LINE_LENGTH && line_input[l]; l++);
        if (l >= MAX_LINE_LENGTH) {
            write(STDERR_FILENO, "error: input too long\n", 22);
        }
        while (line_input[l - 1] == '\r' || line_input[l - 1] == '\n') {
            line_input[--l] = 0;
        }
        int r = parse(line_input);
        
        write(STDOUT_FILENO, "$ ", 2);
    }

    return 0;
}