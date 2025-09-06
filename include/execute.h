#ifndef QSH_EXECUTE_H
#define QSH_EXECUTE_H

/**
 * @file execute.h
 * @brief Command execution functions.
 */

#include <stdlib.h>

/**
 * @brief Execute an argv-style command array (may be a builtin or external).
 *
 * @param args NULL-terminated argv array (tokens strdup'd)
 * @return int 1 to continue shell, 0 to exit
 */
int qsh_execute(char **args);

/**
 * @brief Spawn a new process and run args via execvp.
 *
 * @param args NULL-terminated argv array
 * @return always returns 1 to continue shell
 */
int new_process(char **args);

/**
 * @brief Last exit status (0 == success). Updated by execution code.
 *
 * This is global state read by prompt code to show a success/failure indicator.
 */
extern int qsh_last_status;

#endif // QSH_EXECUTE_H
