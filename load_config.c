#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #include "qsh_parse.c"
// #include "qsh_execute.c"

void load_config(const char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (!fp)
        return; // silently skip if file not found

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        // Remove trailing newline
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        // Skip empty or comment lines
        if (line[0] == '\0' || line[0] == '#')
            continue;

        // Split into args and execute
        char **args = qsh_parse(line);
        if (args[0] != NULL)
        {
            qsh_execute(args);
        }
        free(args);
    }

    free(line);
    fclose(fp);
}
