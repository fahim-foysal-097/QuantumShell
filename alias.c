#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALIASES 100

typedef struct
{
    char *name;    // alias name ("ls")
    char *command; // alias command ("ls -la")
} alias_t;

alias_t aliases[MAX_ALIASES];
int alias_count = 0;

char **qsh_parse(char *cmd);

/// @brief Set or update an alias
/// @param name Alias name
/// @param command Command to associate with the alias
void set_alias(const char *name, const char *command)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
        {
            free(aliases[i].command);
            aliases[i].command = strdup(command);
            return;
        }
    }

    if (alias_count < MAX_ALIASES)
    {
        aliases[alias_count].name = strdup(name);
        aliases[alias_count].command = strdup(command);
        alias_count++;
    }
    else
    {
        fprintf(stderr, "qsh: max aliases reached\n");
    }
}

/// @brief Print all aliases
void print_aliases()
{
    for (int i = 0; i < alias_count; i++)
    {
        printf("alias %s='%s'\n", aliases[i].name, aliases[i].command);
    }
}

/// @brief Expand the first argument if it matches an alias
/// @param args Argument vector to potentially modify
void expand_alias(char **args)
{
    if (args[0] == NULL)
        return;

    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(args[0], aliases[i].name) == 0) // check for alias match
        {
            char **alias_args = qsh_parse(aliases[i].command);

            // shift original args after replacement
            int j = 0;
            while (alias_args[j] != NULL)
            {
                args[j] = alias_args[j];
                j++;
            }
            args[j] = NULL; // terminate args
            break;
        }
    }
}

/// @brief Builtin alias command
/// @param args Argument vector
/// @return Always returns 1 to indicate shell should continue
int qsh_alias(char **args)
{
    if (args[1] == NULL)
    {
        print_aliases();
    }
    else
    {
        char *eq = NULL;
        // Search for '=' across all args
        for (int i = 1; args[i]; i++)
        {
            eq = strchr(args[i], '=');
            if (eq)
            {
                // Merge rest of args after '=' as command
                char name[64];
                strncpy(name, args[i], eq - args[i]);
                name[eq - args[i]] = '\0';

                // build full command
                char command[1024] = "";
                if (*(eq + 1) != '\0')
                    strcat(command, eq + 1); // part after =
                for (int j = i + 1; args[j]; j++)
                {
                    strcat(command, " ");
                    strcat(command, args[j]);
                }

                // remove quotes if present
                if (command[0] == '\'' || command[0] == '"')
                {
                    size_t len = strlen(command);
                    if (command[len - 1] == command[0])
                    {
                        command[len - 1] = '\0';
                        memmove(command, command + 1, len - 1);
                    }
                }

                set_alias(name, command);
                return 1;
            }
        }

        fprintf(stderr, "Usage: alias name='command'\n");
    }
    return 1;
}
