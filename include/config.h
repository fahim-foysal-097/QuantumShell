#ifndef QSH_CONFIG_H
#define QSH_CONFIG_H

/**
 * @file config.h
 * @brief Configuration file loader
 */

/**
 * @brief Load a config file once during startup.
 *
 * Reads file line-by-line and executes commands found there.
 *
 * @param filename path to config file (e.g. ".qshrc")
 */
void load_config(const char *filename);

#endif // QSH_CONFIG_H
