#include <stdio.h>

// RPEL - Read, Parse, Execute, Loop

int main(void)
{
    char *cmd;
    char **args;
    int status;

    do
    {
        printf("qsh> ");
        cmd = qsh_read();
        args = qsh_parse(cmd);
        status = qsh_staus(args);

        free(cmd);
        free(args);
        free(status);
    } while (status);
}
