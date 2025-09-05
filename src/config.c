#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include "parse.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Load a config file once at startup (or when invoked via source).
 *
 * Each non-empty, non-comment line is parsed and executed.
 */
void load_config_once(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return; /* silently ignore missing config */

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (read > 0 && line[read - 1] == '\n')
            line[read - 1] = '\0';

        /* skip empty or commented lines */
        char *p = line;
        while (*p == ' ' || *p == '\t')
            ++p;
        if (*p == '\0' || *p == '#')
            continue;

        char **args = qsh_parse(p);
        if (args && args[0])
            qsh_execute(args);
        free_args(args);
    }
    free(line);
    fclose(fp);
}
