#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

#define INITIAL_TOK_BUFSIZE 8

char **qsh_parse(const char *cmd)
{
    if (!cmd)
        return NULL;

    size_t bufsize = INITIAL_TOK_BUFSIZE;
    size_t position = 0;
    char **args = malloc(bufsize * sizeof(char *));
    if (!args)
    {
        perror("qsh: malloc");
        return NULL;
    }

    size_t i = 0;
    while (cmd[i])
    {
        /* skip whitespace (space + tab) */
        while (cmd[i] && (cmd[i] == ' ' || cmd[i] == '\t'))
            i++;
        if (!cmd[i])
            break;

        const char *start;
        char quote = 0;

        if (cmd[i] == '\'' || cmd[i] == '"')
        {
            quote = cmd[i++];
            start = &cmd[i];
            /* scan until matching quote or end */
            while (cmd[i] && cmd[i] != quote)
                i++;
            /* note: if cmd[i] == '\0' here -> unmatched quote; we treat token until EOL */
        }
        else
        {
            start = &cmd[i];
            while (cmd[i] && cmd[i] != ' ' && cmd[i] != '\t')
                i++;
        }

        size_t len = (size_t)(&cmd[i] - start);
        char *token = malloc(len + 1);
        if (!token)
        {
            perror("qsh: malloc");
            /* cleanup */
            for (size_t k = 0; k < position; ++k)
                free(args[k]);
            free(args);
            return NULL;
        }
        if (len > 0)
            memcpy(token, start, len);
        token[len] = '\0';

        /* append token */
        args[position++] = token;

        /* if we stopped at a closing quote, skip it */
        if (quote && cmd[i] == quote)
            i++;

        /* grow array if needed (double strategy) */
        if (position + 1 >= bufsize)
        { /* +1 to reserve space for final NULL */
            size_t newsize = bufsize * 2;
            char **tmp = realloc(args, newsize * sizeof(char *));
            if (!tmp)
            {
                perror("qsh: realloc");
                /* cleanup */
                for (size_t k = 0; k < position; ++k)
                    free(args[k]);
                free(args);
                return NULL;
            }
            args = tmp;
            bufsize = newsize;
        }
    }

    args[position] = NULL;
    return args;
}

void free_args(char **args)
{
    if (!args)
        return;
    for (size_t i = 0; args[i]; ++i)
        free(args[i]);
    free(args);
}
