#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

char *qsh_read()
{
    char *buffer = malloc(BUFSIZE * sizeof(char));
    char c;
    int position = 0;

    if (!buffer)
    {
        fprintf(stderr, "qsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    c = getchar();
    while (c != '\n' && c != EOF)
    {
        buffer[position] = c;
        position++;
        if (position >= BUFSIZE)
        {
            buffer = realloc(buffer, BUFSIZE);
            if (!buffer)
            {
                fprintf(stderr, "qsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        c = getchar();
    }

    printf("You entered: %s\n", buffer);
    return buffer;
}

int main(void)
{
    char *cmd = qsh_read();
    free(cmd);
    return 0;
}
