#include <stdio.h>
#include "qsh_execute.c"
#include "qsh_parse.c"
#include "qsh_read.c"
#include "load_config.c"

void qsh_loop(void);

int main(void)
{

    // char *args[] = {"pwd", NULL};
    // new_process(args);
    load_config(".qshrc");

    qsh_loop();
    return EXIT_SUCCESS;
}

/// @brief Main loop for the QuantumShell.
/// Continuously reads, parses, and executes commands until exit.
/// @param None
void qsh_loop(void)
{
    char *cmd;
    char **args;
    int status;

    do
    {
        printf("\n");
        char *a[] = {"", NULL};
        qsh_pwd(a);

        printf("qsh> ");
        cmd = qsh_read();
        args = qsh_parse(cmd);
        status = qsh_execute(args);

        free(cmd);
        free(args);
    } while (status);
}
