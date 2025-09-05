// Functions for built-in shell commands.

#include <stdio.h>
#include <unistd.h>
#include "alias.c"

char *func_list[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "alias",
};

/// @brief Changes the current working directory.
/// @param args Arguments passed to the cd command.
/// @return 1.
int qsh_cd(char **args)
{
    if (args[1] == NULL || "~")
    {
        chdir(getenv("HOME"));
    }
    else if (args[1] == "-")
    {
        chdir(getenv("OLDPWD"));
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("qsh");
        }
    }
    return 1;
}

/// @brief Prints the list of built-in commands.
/// @param args Arguments passed to the function (unused).
/// @return 1.
int qsh_help(char **args)
{
    printf("qsh: Quantumn shell\n");
    printf("Built-in commands:\n");
    printf("  cd [dir]      Change the current directory.\n");
    printf("  help          Display this help message.\n");
    printf("  exit          Exit the shell.\n");
    printf("  pwd           Print wroking directory.\n");
    // printf("  env           Print environment variables.\n");

    return 1;
}

/// @brief Exits the shell.
/// @param args Arguments passed to the exit command (unused).
/// @return 0 to signal shell termination.
int qsh_exit(char **args)
{
    return 0;
}

/// @brief Prints the current working directory.
/// @param args Arguments passed to the pwd command (unused).
/// @return 1.
int qsh_pwd(char **args)
{

    if (args[1] != NULL)
    {
        fprintf(stderr, "pwd: too many arguments\n");
        return 1;
    }

    char *dir = getcwd(NULL, 0);
    if (dir == NULL)
    {
        perror("qsh");
    }
    else
    {
        printf("%s\n", dir);
        free(dir);
    }
    return 1;
}
