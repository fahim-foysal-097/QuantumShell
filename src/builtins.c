#define _POSIX_C_SOURCE 200809L
#include "builtins.h"
#include "parse.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
    printf("qsh: Quantum shell --- A simple shell\n");
    printf("Builtins:\n");
    printf("  cd [dir]              change directory\n");
    printf("  help                  display this help message\n");
    printf("  exit                  exit the shell\n");
    printf("  pwd                   print working directory\n");
    printf("  alias                 list aliases\n");
    printf("  source <file>         source a configuration file\n");
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

/**
 * @brief Echo builtin.
 *
 * Supports:
 *   - "-n" option: do not print trailing newline.
 * @param args Null-terminated argument vector. args[0] = "echo"
 * @return Always returns 1 (shell should continue)
 */
int qsh_echo(char **args)
{
    if (!args || !args[0])
        return 1;

    int start = 1;       // first argument to print
    bool newline = true; // print newline by default

    // Check for -n
    if (args[1] && strcmp(args[1], "-n") == 0)
    {
        newline = false;
        start = 2; // skip -n
    }

    for (int i = start; args[i]; i++)
    {
        printf("%s", args[i]);
        if (args[i + 1])
            putchar(' ');
    }

    if (newline)
        putchar('\n');

    return 1;
}
