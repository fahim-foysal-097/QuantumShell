#include <stdio.h>
#include <stdlib.h>

#define BUFFFSIZE 1

char *qsh_read()
{
    size_t bufsize = BUFFFSIZE;
    char *buffer = malloc(bufsize * sizeof(char));
    int position = 0;
    int c;

    if (!buffer)
    {
        fprintf(stderr, "qsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while ((c = getchar()) != '\n' && c != EOF)
    {
        buffer[position] = c;
        position++;
        if (position >= bufsize)
        {
            bufsize += BUFFFSIZE;
            buffer = realloc(buffer, bufsize * sizeof(char));
            if (!buffer)
            {
                fprintf(stderr, "qsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    buffer[position] = '\0';

    printf("You entered: %s\n", buffer);

    return buffer;
}

int main(void)
{
    char *cmd = qsh_read();
    free(cmd);
    return 0;
}
