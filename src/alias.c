#define _POSIX_C_SOURCE 200809L
#include "alias.h"
#include "execute.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALIASES 100

typedef struct
{
    char *name;
    char *command;
} alias_t;

static alias_t aliases[MAX_ALIASES];
static int alias_count = 0;

static char *trim_inplace(char *s);
static void strip_quotes_inplace(char *s);

char **expand_alias(char **args)
{
    if (!args || !args[0])
        return args;

    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(args[0], aliases[i].name) == 0)
        {
            /* parse alias command into tokens */
            char **alias_args = qsh_parse(aliases[i].command);
            if (!alias_args)
                return args; /* leave original intact */

            int alias_len = 0;
            while (alias_args[alias_len])
                ++alias_len;

            int extra_len = 0;
            while (args[1 + extra_len])
                ++extra_len;

            int new_len = alias_len + extra_len;
            char **new_args = malloc((new_len + 1) * sizeof(char *));
            if (!new_args)
            {
                perror("malloc");
                free_args(alias_args);
                return args; /* degrade gracefully */
            }

            int j = 0;
            /* copy alias tokens */
            for (; j < alias_len; j++)
            {
                new_args[j] = strdup(alias_args[j]);
                if (!new_args[j])
                {
                    perror("strdup");
                    /* rollback everything allocated so far */
                    for (int k = 0; k < j; k++)
                        free(new_args[k]);
                    free(new_args);
                    free_args(alias_args);
                    return args;
                }
            }
            /* copy extra user tokens */
            for (int u = 0; u < extra_len; u++, j++)
            {
                new_args[j] = strdup(args[1 + u]);
                if (!new_args[j])
                {
                    perror("strdup");
                    for (int k = 0; k < j; k++)
                        free(new_args[k]);
                    free(new_args);
                    free_args(alias_args);
                    return args;
                }
            }
            new_args[new_len] = NULL;

            free_args(alias_args);
            return new_args;
        }
    }
    return args;
}

void set_alias(const char *name, const char *command)
{
    if (!name || !command)
        return;

    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
        {
            char *new_cmd = strdup(command);
            if (!new_cmd)
            {
                perror("strdup");
                return;
            }
            free(aliases[i].command);
            aliases[i].command = new_cmd;
            return;
        }
    }

    if (alias_count < MAX_ALIASES)
    {
        char *n = strdup(name);
        char *c = strdup(command);
        if (!n || !c)
        {
            perror("strdup");
            free(n);
            free(c);
            return;
        }
        aliases[alias_count].name = n;
        aliases[alias_count].command = c;
        alias_count++;
    }
    else
    {
        fprintf(stderr, "qsh: max aliases reached\n");
    }
}

void print_aliases(void)
{
    for (int i = 0; i < alias_count; i++)
    {
        printf("alias %s='%s'\n", aliases[i].name, aliases[i].command);
    }
}

int qsh_alias_builtin(char **args)
{
    if (!args || !args[1])
    {
        print_aliases();
        qsh_last_status = 0;
        return 1;
    }

    /* join args[1..] into a single string */

    size_t total_len = 1; // +1 for null terminator
    for (int i = 1; args[i]; i++)
        total_len += strlen(args[i]) + 1; // +1 extra for a space separator between arg

    char *joined_args = malloc(total_len); // buffer that will hold the joined arguments.
    if (!joined_args)
    {
        perror("malloc");
        return 1;
    }
    joined_args[0] = '\0';

    for (int i = 1; args[i]; i++)
    {
        if (i > 1)
            strcat(joined_args, " ");
        strcat(joined_args, args[i]);
    }

    char *equal_sign = strchr(joined_args, '=');
    if (!equal_sign)
    {
        fprintf(stderr, "Usage: alias name='command'\n");
        free(joined_args);
        qsh_last_status = 1;
        return 1;
    }

    *equal_sign = '\0';                           // terminate the alias name
    char *name = trim_inplace(joined_args);       // remove leading/trailing whitespace
    char *command = trim_inplace(equal_sign + 1); // extract command part
    strip_quotes_inplace(command);                // remove surrounding quotes

    if (name[0] == '\0' || command[0] == '\0')
    {
        fprintf(stderr, "Usage: alias name='command'\n");
        free(joined_args);
        qsh_last_status = 1;
        return 1;
    }

    set_alias(name, command);
    free(joined_args);
    qsh_last_status = 0;
    return 1;
}

void alias_cleanup(void)
{
    for (int i = 0; i < alias_count; i++)
    {
        free(aliases[i].name);
        free(aliases[i].command);
        aliases[i].name = NULL;
        aliases[i].command = NULL;
    }
    alias_count = 0;
}

/**
 * @brief Remove leading and trailing spaces/tabs from a string in place.
 * @param str The string to trim. Must be null-terminated.
 * @return Pointer to the trimmed string (same as input).
 */
static char *trim_inplace(char *str)
{
    if (!str)
        return NULL;

    // --- Trim leading spaces/tabs ---
    char *read_ptr = str; // pointer to scan from the start
    while (*read_ptr && (*read_ptr == ' ' || *read_ptr == '\t'))
        ++read_ptr;

    // Shift string left if there were leading spaces
    if (read_ptr != str)
        memmove(str, read_ptr, strlen(read_ptr) + 1); // include null terminator

    // --- Trim trailing spaces/tabs ---
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t'))
        str[--len] = '\0'; // overwrite trailing whitespace with null terminator

    return str;
}

/**
 * @brief Remove surrounding single or double quotes from a string in place.
 * @param str The string to process. Must be null-terminated.
 */
static void strip_quotes_inplace(char *str)
{
    if (!str)
        return;

    size_t len = strlen(str);
    if (len < 2)
        return; // too short to have quotes

    char first = str[0];
    char last = str[len - 1];

    // Check if string starts and ends with the same type of quote
    if ((first == '\'' && last == '\'') || (first == '"' && last == '"'))
    {
        str[len - 1] = '\0';            // remove trailing quote
        memmove(str, str + 1, len - 1); // shift left to remove leading quote
    }
}
