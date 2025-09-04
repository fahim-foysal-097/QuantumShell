#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 1024
#define DELIM " \t\r\n\a"

char **qsr_parse(char *cmd)
{
    char **buffer = malloc(BUFSIZE * sizeof(char *));
    int position = 0;

    char *arg = strtok(cmd, DELIM);

    while (arg != NULL)
    {
        buffer[position] = arg;
        position++;
        arg = strtok(NULL, DELIM);
    }

    for (int i = 0; i < position; i++)
    {
        printf("Parsed : %s\n", buffer[i]);
    }

    return buffer;
}

int main(void)
{
    char cmd[] = "ls -la /cd";
    qsr_parse(cmd);
    free(cmd);
    return 0;
}
