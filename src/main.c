#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "read.h"
#include "execute.h"
#include "alias.h"
#include "config.h"
#include "builtins.h"

static void qsh_loop(void);

int main(void)
{
    /* load config at startup */
    char config_path[1024];
    const char *home = getenv("HOME"); // get home directory

    if (home)
    {
        snprintf(config_path, sizeof(config_path), "%s/.qshrc", home);
        load_config(config_path);
    }

    /* main loop */
    qsh_loop();

    return EXIT_SUCCESS;
}

/**
 * @brief Main loop: read -> parse -> execute --> loop
 */
static void qsh_loop(void)
{
    char *cmd;
    char **args;
    int status = 1;

    do
    {
        /* print working dir */
        char *a[] = {"", NULL};
        qsh_pwd(a);

        printf("qsh> ");
        cmd = qsh_read();
        if (!cmd)
            break; /* EOF */

        if (cmd[0] == '\0')
        {
            free(cmd);
            continue;
        }

        args = qsh_parse(cmd);
        status = qsh_execute(args);

        free(cmd);
        free_args(args);
    } while (status);

    /* cleanup alias storage */
    alias_cleanup();
}
