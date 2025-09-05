#ifndef QSH_READ_H
#define QSH_READ_H

/**
 * @file read.h
 * @brief Input reading utilities.
 */

#include <stdlib.h>

/**
 * @brief Read a line from stdin and return it as a malloc'd string.
 *
 * Caller must free the returned pointer. Returns empty string ("") if user
 * pressed enter. Returns NULL on EOF with no data.
 *
 * @return char* malloc'd line (without newline) or NULL on EOF.
 */
char *qsh_read(void);

#endif // QSH_READ_H
