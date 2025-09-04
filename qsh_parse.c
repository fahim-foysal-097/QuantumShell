#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 1024
#define DELIM " \t\r\n\a"

char **qsr_parse(char *cmd)
{
    int position = 0;
    size_t bufsize = BUFSIZE;
    char **args = malloc(bufsize * sizeof(char *));

    char *arg = strtok(cmd, DELIM);

    while (arg != NULL)
    {
        args[position] = arg;
        position++;
        if (position >= bufsize)
        {
            bufsize += BUFSIZE;
            args = realloc(args, bufsize * sizeof(char *));
            if (!args)
            {
                fprintf(stderr, "qsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        arg = strtok(NULL, DELIM);
    }

    args[position] = NULL;

    for (int i = 0; i < position; i++)
    {
        printf("Parsed : %s\n", args[i]);
    }

    return args;
}

int main(void)
{
    char cmd[] = "ls -la /cd keyboard";
    char **args = qsr_parse(cmd);
    free(args);
    return 0;
}
