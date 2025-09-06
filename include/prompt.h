#ifndef QSH_PROMPT_H
#define QSH_PROMPT_H

/**
 * @file prompt.h
 * @brief Build a modern prompt string for the shell.
 *
 * The returned string is malloc'd and must be freed by the caller.
 */

#include <stdlib.h>

/**
 * @brief Build the prompt string.
 *
 * The returned pointer is a malloc'd, NUL-terminated C string. Caller must free.
 * The prompt contains color escapes only when stdout is a TTY.
 *
 * Example output (single line):
 *   user@host:~/projects/foo ( main) 14:23
 *   λ
 *
 * @return char* malloc'd prompt string, or NULL on error.
 */
char *build_prompt(void);

#endif // QSH_PROMPT_H
