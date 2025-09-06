#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

#define DELIM " \t\r\n\a"
#define INITIAL_TOK_BUFSIZE 8

char **qsh_parse(const char *cmd)
{
    if (!cmd)
        return NULL;

    char *tmp = strdup(cmd);
    if (!tmp)
    {
        perror("strdup");
        return NULL;
    }

    size_t bufsize = INITIAL_TOK_BUFSIZE;
    char **args = malloc(bufsize * sizeof(char *));
    if (!args)
    {
        fprintf(stderr, "Memory allocation failed\n");
        free(tmp);
        return NULL;
    }

    int position = 0;
    char *saveptr = NULL;
    char *token = strtok_r(tmp, DELIM, &saveptr);
    while (token)
    {
        if (position >= (int)bufsize)
        {
            bufsize += INITIAL_TOK_BUFSIZE;
            char **tmpa = realloc(args, bufsize * sizeof(char *));
            if (!tmpa)
            {
                fprintf(stderr, "Memory reallocation failed\n");
                /* roll back safely: free only what we allocated */
                for (int i = 0; i < position; i++)
                    free(args[i]);
                free(args);
                free(tmp);
                return NULL;
            }
            args = tmpa;
        }
        char *dup = strdup(token);
        if (!dup)
        {
            perror("strdup");
            /* roll back safely */
            for (int i = 0; i < position; i++)
                free(args[i]);
            free(args);
            free(tmp);
            return NULL;
        }
        args[position] = dup;
        position++;
        token = strtok_r(NULL, DELIM, &saveptr);
    }

    /* add terminator */
    if (position >= (int)bufsize)
    {
        char **tmpa = realloc(args, (bufsize + 1) * sizeof(char *));
        if (!tmpa)
        {
            perror("realloc");
            for (int i = 0; i < position; i++)
                free(args[i]);
            free(args);
            free(tmp);
            return NULL;
        }
        args = tmpa;
    }
    args[position] = NULL;

    free(tmp);
    return args;
}

void free_args(char **args)
{
    if (!args)
        return;
    for (int i = 0; args[i]; i++)
    {
        free(args[i]);
    }
    free(args);
}
