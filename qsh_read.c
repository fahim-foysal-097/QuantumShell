#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

/**
 * @brief Reads a line of input from stdin.
 * @return Pointer to the dynamically allocated input string. Caller must free.
 */
char *qsh_read()
{
    size_t bufsize = BUFSIZE;
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
            bufsize += BUFSIZE;
            buffer = realloc(buffer, bufsize * sizeof(char));
            if (!buffer)
            {
                fprintf(stderr, "qsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    buffer[position] = '\0';

    // printf("You entered: %s\n", buffer);

    return buffer;
}

// int main(void)
// {
//     char *cmd = qsh_read();
//     free(cmd);
//     return 0;
// }
