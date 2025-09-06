#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"
#include "config.h"
#include "execute.h"
#include "parse.h"

/**
 * @brief built-in cd.
 *
 * - Supports:
 *   - Quoted paths: cd 'A Long Folder' (also " ") &
 *   - Expand leading ~ (tilde) if present: ~ or ~/something
 * @return Always returns 1 (shell should continue)
 */
int qsh_cd(char **args)
{
    qsh_last_status = 0; /* assume success */

    if (!args || !args[1] || strcmp(args[1], "~") == 0)
    {
        const char *home = getenv("HOME");
        if (!home)
            home = "/";
        if (chdir(home) != 0)
        {
            perror("qsh");
            qsh_last_status = 1;
        }
        return 1;
    }

    if (strcmp(args[1], "-") == 0)
    {
        const char *oldpwd = getenv("OLDPWD");
        if (oldpwd)
        {
            if (chdir(oldpwd) != 0)
            {
                perror("qsh");
                qsh_last_status = 1;
            }
        }
        else
        {
            fprintf(stderr, "qsh: OLDPWD not set\n");
            qsh_last_status = 1;
        }
        return 1;
    }

    /* Handle quoted paths*/
    char *raw = NULL;
    char quote = '\0';

    if (args[1][0] == '\'' || args[1][0] == '"')
    {
        quote = args[1][0];

        size_t tlen = strlen(args[1]);

        /* Case: single token contains both quotes: e.g. 'foo' */
        if (tlen > 1 && args[1][tlen - 1] == quote)
        {
            /* copy inner content without the surrounding quotes */
            size_t inner_len = tlen - 2;
            raw = malloc(inner_len + 1);
            if (!raw)
            {
                perror("malloc");
                qsh_last_status = 1;
                return 1;
            }
            if (inner_len > 0)
                memcpy(raw, args[1] + 1, inner_len);
            raw[inner_len] = '\0';
        }
        else
        {
            /*concatenate tokens until the closing quote */
            size_t total = 0;
            int end_index = -1;
            for (int i = 1; args[i]; ++i)
            {
                total += strlen(args[i]) + 1; /* +1 for potential space */
                size_t li = strlen(args[i]);
                if (li > 0 && args[i][li - 1] == quote)
                {
                    end_index = i;
                    break;
                }
            }

            if (end_index == -1)
            {
                fprintf(stderr, "qsh: unmatched quote in cd argument\n");
                qsh_last_status = 1;
                return 1;
            }

            raw = malloc(total + 1);
            if (!raw)
            {
                perror("malloc");
                qsh_last_status = 1;
                return 1;
            }
            raw[0] = '\0';

            for (int j = 1; j <= end_index; ++j)
            {
                if (j > 1)
                    strcat(raw, " ");
                if (j == 1)
                {
                    /* skip leading quote of first token */
                    if (args[j][0] == quote)
                        strcat(raw, args[j] + 1);
                    else
                        strcat(raw, args[j]);
                }
                else
                {
                    strcat(raw, args[j]);
                }
            }

            /* Now strip trailing quote if present */
            size_t rlen = strlen(raw);
            if (rlen > 0 && raw[rlen - 1] == quote)
                raw[rlen - 1] = '\0';
        }
    }
    else
    {
        /* Unquoted simple token - copy it to heap for consistent ownership */
        raw = strdup(args[1]);
        if (!raw)
        {
            perror("strdup");
            qsh_last_status = 1;
            return 1;
        }
    }

    /* Expand leading ~ (tilde) if present: ~ or ~/something */
    char *final_path = raw;
    if (raw[0] == '~')
    {
        const char *home = getenv("HOME");
        if (!home)
            home = "/";

        /* raw may be "~" or "~/rest" */
        if (raw[1] == '\0')
        {
            final_path = strdup(home);
            free(raw);
            if (!final_path)
            {
                perror("malloc");
                qsh_last_status = 1;
                return 1;
            }
        }
        else if (raw[1] == '/')
        {
            size_t need = strlen(home) + strlen(raw); /* home + rest (includes leading ~) */
            char *expanded = malloc(need);            /* enough space */
            if (!expanded)
            {
                perror("malloc");
                free(raw);
                qsh_last_status = 1;
                return 1;
            }
            /* concatenate: home + (raw + 1) to skip leading ~ */
            strcpy(expanded, home);
            strcat(expanded, raw + 1);
            free(raw);
            final_path = expanded;
        }
        /* else: something like ~user/... not supported - leave as-is */
    }

    /* Attempt to chdir */
    if (chdir(final_path) != 0)
    {
        perror("qsh");
        qsh_last_status = 1;
    }
    else
    {
        qsh_last_status = 0;
    }

    free(final_path);

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
    qsh_last_status = 0;
    return 0;
}

/* built-in pwd */
int qsh_pwd(char **args)
{
    (void)args;
    char *dir = getcwd(NULL, 0);
    if (!dir)
    {
        perror("qsh");
        qsh_last_status = 1;
    }
    else
    {
        printf("%s\n", dir);
        free(dir);
        qsh_last_status = 0;
    }
    return 1;
}

/* source builtin: uses load_config_once on given file */
int qsh_source(char **args)
{
    const char *file = ".qshrc";
    if (args && args[1])
        file = args[1];
    load_config(file);
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
