#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "vars.h"

/**
 * @brief Append formatted data to a dynamically growing buffer.
 * Ensures buffer has enough space for new content.
 *
 * @return 0 on success, -1 on allocation error.
 */
static int append_str(char **buf, size_t *buflen, size_t *len, const char *src)
{
    size_t need = strlen(src);
    if (*len + need + 1 > *buflen)
    {
        size_t newlen = (*buflen == 0) ? (need + 32) : (*buflen * 2);

        while (*len + need + 1 > newlen)
            newlen *= 2;

        char *tmp = realloc(*buf, newlen);

        if (!tmp)
            return -1;

        *buf = tmp;
        *buflen = newlen;
    }
    memcpy(*buf + *len, src, need);
    *len += need;
    (*buf)[*len] = '\0';
    return 0;
}

/**
 * @brief Append a single character to buffer.
 */
static int append_char(char **buf, size_t *buflen, size_t *len, char c)
{
    if (*len + 2 > *buflen)
    {
        size_t newlen = (*buflen == 0) ? 64 : (*buflen * 2);
        char *tmp = realloc(*buf, newlen);

        if (!tmp)
            return -1;
        *buf = tmp;
        *buflen = newlen;
    }
    (*buf)[(*len)++] = c;
    (*buf)[*len] = '\0';
    return 0;
}

/**
 * @brief Expand variables inside a single token string.
 *
 * @return a freshly malloc'd string (caller must free). On allocation error,
 * returns an empty strdup("") (never NULL to simplify callers).
 */
static char *expand_token(const char *s)
{
    if (!s)
        return strdup("");

    size_t slen = strlen(s);
    char *out = NULL;
    size_t out_len = 0;
    size_t out_cap = 0;

    for (size_t i = 0; i < slen; ++i)
    {
        char c = s[i];
        if (c == '$')
        {
            /* handle $$ -> PID */
            if (i + 1 < slen && s[i + 1] == '$')
            {
                char pidbuf[32];
                snprintf(pidbuf, sizeof(pidbuf), "%d", (int)getpid());

                if (append_str(&out, &out_cap, &out_len, pidbuf) < 0)
                    goto oom;
                i++; /* consumed second $ */
                continue;
            }

            /* ${VAR} form */
            if (i + 1 < slen && s[i + 1] == '{')
            {
                size_t j = i + 2;
                while (j < slen && s[j] != '}')
                    j++;
                if (j >= slen)
                {
                    /* unmatched brace: treat literally '${' */
                    if (append_str(&out, &out_cap, &out_len, "${") < 0)
                        goto oom;
                    i++; /* skip the '{' and continue after */
                    continue;
                }
                /* name is s[i+2 .. j-1] */
                size_t namelen = j - (i + 2);
                char name[namelen + 1];
                memcpy(name, s + i + 2, namelen);
                name[namelen] = '\0';
                const char *val = getenv(name);

                if (val)
                {
                    if (append_str(&out, &out_cap, &out_len, val) < 0)
                        goto oom;
                }
                i = j; /* advance past '}' */
                continue;
            }

            /* $VAR form: VAR must start with letter or underscore */
            if (i + 1 < slen && (isalpha((unsigned char)s[i + 1]) || s[i + 1] == '_'))
            {
                size_t j = i + 2;
                while (j < slen && (isalnum((unsigned char)s[j]) || s[j] == '_'))
                    j++;
                size_t namelen = j - (i + 1);
                char name[namelen + 1];
                memcpy(name, s + i + 1, namelen);
                name[namelen] = '\0';
                const char *val = getenv(name);
                if (val)
                {
                    if (append_str(&out, &out_cap, &out_len, val) < 0)
                        goto oom;
                }
                i = j - 1;
                continue;
            }

            /* otherwise, not a variable reference; emit '$' literally */
            if (append_char(&out, &out_cap, &out_len, '$') < 0)
                goto oom;
            continue;
        }

        /* normal character */
        if (append_char(&out, &out_cap, &out_len, c) < 0)
            goto oom;
    }

    /* finalize: ensure NUL */
    if (!out)
    {
        out = strdup("");
        if (!out)
            return NULL;
    }
    return out;

/* out-of-memory */
oom:
    /* allocation failure: free partial and return empty string */
    free(out);
    return strdup("");
}

void expand_variables_inplace(char **args)
{
    if (!args)
        return;

    for (int i = 0; args[i]; ++i)
    {
        char *old = args[i];
        char *newtok = expand_token(old);

        /* expand_token never returns NULL; but guard anyway */
        if (!newtok)
            newtok = strdup("");

        /* replace in place */
        args[i] = newtok;
        free(old);
    }
}
