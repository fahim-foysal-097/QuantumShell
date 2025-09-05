#ifndef QSH_PARSE_H
#define QSH_PARSE_H

/**
 * @file parse.h
 * @brief Command parsing utilities.
 */

#include <stdlib.h>

/**
 * @brief Parse a command string into an argv-style NULL-terminated array.
 *
 * Each token is allocated with strdup() and the returned array must be freed
 * with free_args().
 *
 * @param cmd Command string to parse (may be const).
 * @return char** NULL-terminated array of strdup'd tokens or NULL on error.
 */
char **qsh_parse(const char *cmd);

/**
 * @brief Free an argv-style array returned by qsh_parse or other helpers.
 *
 * Frees each token and then frees the array pointer.
 *
 * @param args NULL-terminated array to free; safe to pass NULL.
 */
void free_args(char **args);

#endif // QSH_PARSE_H
