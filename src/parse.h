#ifndef _INC_PARSE_H_
#define _INC_PARSE_H_

struct CommandRedirect {
    char *cmd;
    int stdin_des;
    int stdout_des;
    int unused_pipe;
    int pid;
};

int parse(char *input);


#endif