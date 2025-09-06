#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "alias.h"
#include "builtins.h"
#include "execute.h"
#include "parse.h"

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

/* Global last status (0 == success). Visible to prompt.c via execute.h */
int qsh_last_status = 0;

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
            /* _exit to avoid flushing stdio buffers owned by the parent. */
            _exit(127);
        }
    }
    else if (pid < 0)
    {
        // Error forking
        perror("qsh");
        /* update last status to indicate failure to spawn */
        qsh_last_status = 1;
    }
    else
    {
        // Parent process
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (wpid == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        if (WIFEXITED(status))
        {
            qsh_last_status = WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status))
        {
            /* Common convention: 128 + signal number */
            qsh_last_status = 128 + WTERMSIG(status);
        }
        else
        {
            qsh_last_status = 1; /* generic failure */
        }
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
