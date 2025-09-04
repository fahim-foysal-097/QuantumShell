// Functions for built-in shell commands.

#include <stdio.h>

char *func_list[] = {
    "cd",
    "help",
    "exit",
};

int qsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        chdir(getenv("HOME"));
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

int qsh_help(char **args)
{
    printf("qsh: Quantumn shell\n");
    printf("Built-in commands:\n");
    printf("  cd [dir]      Change the current directory.\n");
    printf("  help          Display this help message.\n");
    printf("  exit          Exit the shell.\n");
    printf("  env           Print environment variables.\n");

    return 1;
}

int qsh_exit(char **args)
{
    return 0;
}
