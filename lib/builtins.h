#ifndef QSH_BUILTINS_H
#define QSH_BUILTINS_H

/**
 * @file builtins.h
 * @brief Builtin command declarations
 */

#include <stdlib.h>

/* Builtin functions signatures (argv-style arrays) */
int qsh_cd(char **args);
int qsh_help(char **args);
int qsh_exit(char **args);
int qsh_pwd(char **args);
int qsh_source(char **args);

#endif // QSH_BUILTINS_H
