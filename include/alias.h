#ifndef QSH_ALIAS_H
#define QSH_ALIAS_H

/**
 * @file alias.h
 * @brief Alias management and expansion.
 */

#include <stdlib.h>

/**
 * @brief Expand alias for args.
 *
 * If args[0] matches an alias, returns a NEW argv-style array containing the
 * alias tokens followed by any extra user tokens (args[1..]). The returned
 * array is malloc/strdup'ed and must be freed with free_args(). If no alias
 * matched, returns the original args pointer unchanged.
 *
 * @param args Original argv-style array (tokens are strdup'd).
 * @return char** New args array (caller must free if different) or original.
 */
char **expand_alias(char **args);

/**
 * @brief Set or update an alias.
 *
 * Makes its own copies of name and command.
 *
 * @param name alias name
 * @param command alias command string
 */
void set_alias(const char *name, const char *command);

/**
 * @brief Print all aliases to stdout.
 */
void print_aliases(void);

/**
 * @brief Builtin handler for alias command.
 *
 * Usage:
 *   alias -> prints aliases
 * 
 *   alias name='cmd with args' -> sets alias
 *
 * @param args argv-style array
 * @return int return code (1 to continue shell)
 */
int qsh_alias_builtin(char **args);

/**
 * @brief Free alias storage; call on exit.
 */
void alias_cleanup(void);

#endif // QSH_ALIAS_H
