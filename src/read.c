#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "read.h"

#define READ_BUFSIZE 1024

char *qsh_read(void)
{
    size_t bufsize = READ_BUFSIZE;
    char *buffer = malloc(bufsize * sizeof(char));
    if (!buffer)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    size_t position = 0; // unsigned long
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        buffer[position] = c;
        position++;
        if (position + 1 >= bufsize) // +1 for null terminator
        {
            bufsize += READ_BUFSIZE;
            char *tmp = realloc(buffer, bufsize * sizeof(char));
            if (!tmp)
            {
                fprintf(stderr, "Memory reallocation failed\n");
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }
    }

    /* handle EOF with no input */
    if (c == EOF && position == 0)
    {
        free(buffer);
        return NULL;
    }

    buffer[position] = '\0';
    return buffer;
}
