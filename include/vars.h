#ifndef QSH_VARS_H
#define QSH_VARS_H

/**
 * @file vars.h
 * @brief Environment variable expansion utilities.
 *
 * The parsing in qsh produces an argv-style array of tokens (each strdup'd).
 * expand_variables_inplace() walks that array and replaces each token with a
 * new malloc'd string with environment variables expanded. The old token
 * pointer is freed. The function never replaces the array pointer itself.
 */

#include <stdlib.h>

/**
 * @brief Expand environment variables inside each token of an argv array.
 *
 * Supported forms:
 *  - $VAR        (VAR must start with letter or underscore, followed by letters/digits/underscores)
 *  - ${VAR}      (braced variable)
 *  - $$          (shell PID)
 *
 * If a variable is not set, it expands to an empty string.
 *
 * @param args NULL-terminated argv array (each args[i] must be a heap-allocated cstring).
 *             The function modifies args in place: frees args[i] and replaces it
 *             with a new malloc'd string containing the expansion result.
 */
void expand_variables_inplace(char **args);

#endif // QSH_VARS_H
