#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "builtins.c"

int qsh_execute(char **args)
{
    char *builtin_func_list[] = {
        "cd",
        // "env",
        "help",
        "exit"};

    int (*builtin_func[])(char **) = {
        &qsh_cd,
        // &qsh_env,
        &qsh_help,
        &qsh_exit};

    int i;

    if (args[0] == NULL)
    {
        // empty command
        return 1;
    }

    for (i = 0; i < sizeof(builtin_func_list) / sizeof(char *); i++) // dynamically calculates the number of functions
    {
        if (strcmp(args[0], builtin_func[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return new_process(args);
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
            exit(EXIT_FAILURE);
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

// int main(void)
// {
//     char *args[] = {"ls", NULL};
//     int status = qsh_execute(args);
//     return 0;
// }
