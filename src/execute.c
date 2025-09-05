#define _POSIX_C_SOURCE 200809L
#include "execute.h"
#include "alias.h"
#include "parse.h"
#include "builtins.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Builtin registration (uses functions in builtins.c) */
static char *builtin_names[] = {
    "cd", "help", "exit", "pwd", "alias", "source", "echo"};

extern int qsh_cd(char **);
extern int qsh_help(char **);
extern int qsh_exit(char **);
extern int qsh_pwd(char **);
extern int qsh_alias_builtin(char **);
extern int qsh_source(char **);

static int (*builtin_funcs[])(char **) = {
    &qsh_cd, &qsh_help, &qsh_exit, &qsh_pwd, &qsh_alias_builtin, &qsh_source, &qsh_echo};

// Return the number of built-in functions
int qsh_func_count(void)
{
    return sizeof(builtin_names) / sizeof(char *);
}

int new_process(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("qsh");
            free_args(args); // not important, but cleaner

            /* _exit to avoid flushing stdio buffers owned by the parent. */
            _exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0)
    {
        // Error forking
        perror("qsh");
    }
    else
    {
        // Parent process
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (wpid == -1)
            {
                perror("qsh");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int qsh_execute(char **args)
{
    if (!args || !args[0])
        return 1;

    /* append full alias (if any) command */
    char **expanded_args = expand_alias(args);

    /* check builtins */
    for (int i = 0; i < qsh_func_count(); i++)
    {
        if (strcmp(expanded_args[0], builtin_names[i]) == 0)
        {
            int status = (*builtin_funcs[i])(expanded_args);
            if (expanded_args != args)
                free_args(expanded_args);
            return status;
        }
    }

    /* not builtin -> external command */
    int status = new_process(expanded_args);
    if (expanded_args != args)
        free_args(expanded_args);
    return status;
}
