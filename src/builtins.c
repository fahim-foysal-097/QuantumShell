#define _POSIX_C_SOURCE 200809L
#include "builtins.h"
#include "parse.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* built-in cd */
int qsh_cd(char **args)
{
    if (!args || !args[1] || strcmp(args[1], "~") == 0)
    {
        const char *home = getenv("HOME");
        if (!home)
            home = "/";
        if (chdir(home) != 0)
            perror("qsh");
    }
    else if (strcmp(args[1], "-") == 0)
    {
        const char *oldpwd = getenv("OLDPWD");
        if (oldpwd)
        {
            if (chdir(oldpwd) != 0)
                perror("qsh");
        }
        else
        {
            fprintf(stderr, "qsh: OLDPWD not set\n");
        }
    }
    else
    {
        if (chdir(args[1]) != 0)
            perror("qsh");
    }
    return 1;
}

/* built-in help */
int qsh_help(char **args)
{
    (void)args;
    printf("qsh: Quantum shell (simple)\n");
    printf("Builtins:\n");
    printf("  cd [dir]\n");
    printf("  help\n");
    printf("  exit\n");
    printf("  pwd\n");
    printf("  alias\n");
    printf("  source <file>\n");
    return 1;
}

/* built-in exit */
int qsh_exit(char **args)
{
    (void)args;
    return 0;
}

/* built-in pwd */
int qsh_pwd(char **args)
{
    (void)args;
    char *dir = getcwd(NULL, 0);
    if (!dir)
        perror("qsh");
    else
    {
        printf("%s\n", dir);
        free(dir);
    }
    return 1;
}

/* source built-in: uses load_config_once on given file */
int qsh_source(char **args)
{
    const char *file = ".qshrc";
    if (args && args[1])
        file = args[1];
    load_config_once(file);
    return 1;
}
