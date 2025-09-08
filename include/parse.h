#ifndef QSH_PARSE_H
#define QSH_PARSE_H

/**
 * @file parse.h
 * @brief Command parsing utilities for qsh.
 *
 * qsh_parse() splits a command line into argv-style tokens.
 * - Quoted strings (single '...' or double "..." quotes) are treated as single tokens.
 * - The input string is NOT modified.
 *
 * The returned array and each token are heap-allocated and must be freed
 * by the caller using free_args().
 *
 * Return values:
 *  - On success: a NULL-terminated array of malloc'd strings (args[0] may be NULL for an empty command).
 *  - On allocation failure or if cmd == NULL: returns NULL.
 */

#include <stdlib.h>

/**
 * @brief Parse a command string into tokens.
 *
 * @param cmd NUL-terminated command string to parse (not modified).
 * @return NULL-terminated array of malloc'd token strings, or NULL on error.
 */
char **qsh_parse(const char *cmd);

/**
 * @brief Free an argv-style array returned by qsh_parse.
 *
 * Frees each token and then frees the array pointer. Safe to pass NULL.
 *
 * @param args NULL-terminated array to free.
 */
void free_args(char **args);

#endif // QSH_PARSE_H
