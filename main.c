#include <stdio.h>
#include "qsh_execute.c"
#include "qsh_parse.c"
#include "qsh_read.c"

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
        status = qsh_execute(args);

        free(cmd);
        free(args);
        free(status);
    } while (status);

    return EXIT_SUCCESS;
}
