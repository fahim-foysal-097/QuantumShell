#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "builtin.c"

char *builtin_func_list[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[])(char **) = {
    &qsh_cd,
    &qsh_help,
    &qsh_exit,
};

int qsh_func_count(void)
{
    return sizeof(builtin_func_list) / sizeof(char *);
}

int new_process(char **args);

/**
 * @brief Executes shell built-in or launches a program.
 * @param args Null-terminated array of argument strings.
 * @return Status code: 1 to continue execution, 0 to exit shell.
 */
int qsh_execute(char **args)
{

    int i;

    if (args[0] == NULL)
    {
        // empty command
        return 1;
    }

    for (i = 0; i < qsh_func_count(); i++) // dynamically calculates the number of functions
    {
        if (strcmp(args[0], builtin_func_list[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return new_process(args);
}

/**
 * @brief Creates a new process to execute a command.
 * @param args Null-terminated array of argument strings.
 * @return Status code: 1 to continue execution.
 */
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
