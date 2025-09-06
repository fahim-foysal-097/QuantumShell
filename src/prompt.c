#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "execute.h"
#include "prompt.h"

/* Color codes */
#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define COL_USERHOST "\x1b[1;32m" /* green */
#define COL_CWD "\x1b[1;34m"      /* blue */
#define COL_GIT "\x1b[0;33m"      /* yellow */
#define COL_TIME "\x1b[2;37m"     /* dim gray */
#define COL_ARROW "\x1b[1;37m"    /* bright white */
#define COL_SUCCESS "\x1b[1;32m"  /* green */
#define COL_FAIL "\x1b[1;31m"     /* red */
#define PRE_SIMBL "λ"             /* Prefix*/

/* Checks if stdout is connected to a terminal (TTY). */
static int is_tty(void)
{
    return isatty(STDOUT_FILENO);
}

/* Return a shortened version for path; result is malloc'd */
static char *shorten_cwd(const char *cwd)
{
    if (!cwd)
        return strdup("?");

    const char *home = getenv("HOME");
    size_t cwdlen = strlen(cwd);

    /* Replace home prefix with ~ if present */
    if (home && strncmp(cwd, home, strlen(home)) == 0)
    {
        /* if exactly home, show ~ */
        if (cwd[strlen(home)] == '\0')
        {
            return strdup("~");
        }

        /* build "~/<rest>" */
        size_t rest_len = cwdlen - strlen(home);
        char *out = malloc(1 + 1 + rest_len + 1); /* "~" + "/" + rest + NUL */
        if (!out)
            return NULL;
        out[0] = '~';
        out[1] = '\0';
        strcat(out, cwd + strlen(home));
        return out;
    }

    /* if path short enough, return copy */
    if (cwdlen <= 30)
        return strdup(cwd);

    /* otherwise, ellipsize to last two components */
    const char *p = cwd + cwdlen;
    int slash_count = 0;
    while (p > cwd && slash_count < 2)
    {
        --p;
        if (*p == '/')
            ++slash_count;
    }
    if (p <= cwd)
        return strdup(cwd); /* fallback */

    /* prefix with "…/" */
    size_t tail_len = strlen(p);
    char *out = malloc(4 + tail_len + 1);
    if (!out)
        return NULL;
    strcpy(out, "…/");
    strcat(out, p + (*p == '/' ? 1 : 0));
    return out;
}

/* Get git branch using git command; returns malloc'd string or NULL */
static char *get_git_branch(void)
{
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
    if (!fp)
        return NULL;
    char buf[256];
    if (!fgets(buf, sizeof(buf), fp))
    {
        pclose(fp);
        return NULL;
    }

    pclose(fp);

    /* trim newline */
    size_t n = strlen(buf);
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r'))
    {
        buf[--n] = '\0';
    }
    if (n == 0)
        return NULL;
    return strdup(buf);
}

/* safely get username */
static char *get_user(void)
{
    const char *u = getlogin();
    if (u && u[0])
        return strdup(u);
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_name)
        return strdup(pw->pw_name);
    return strdup("user");
}

/* safely get hostname */
static char *get_host(void)
{
    char buf[HOST_NAME_MAX + 1];
    if (gethostname(buf, sizeof(buf)) == 0)
    {
        buf[sizeof(buf) - 1] = '\0';
        return strdup(buf);
    }
    return strdup("host");
}

char *build_prompt(void)
{
    int tty = is_tty();

    char *user = get_user();
    char *host = get_host();

    char cwd_raw[PATH_MAX];
    if (!getcwd(cwd_raw, sizeof(cwd_raw)))
    {
        strcpy(cwd_raw, "?");
    }
    char *cwd = shorten_cwd(cwd_raw);

    char *branch = get_git_branch(); /* might be NULL */

    /* time */
    char tbuf[6] = "";
    time_t now = time(NULL);
    struct tm tm;
    localtime_r(&now, &tm);
    strftime(tbuf, sizeof(tbuf), "%H:%M", &tm);

    /* Build exit-status indicator */
    char indicator[128] = "";
    if (tty)
    {
        if (qsh_last_status == 0)
        {
            snprintf(indicator, sizeof(indicator), COL_SUCCESS "✔" ANSI_RESET " ");
        }
        else
        {
            snprintf(indicator, sizeof(indicator), COL_FAIL "✘(%d)" ANSI_RESET " ", qsh_last_status);
        }
    }
    else
    {
        if (qsh_last_status == 0)
        {
            snprintf(indicator, sizeof(indicator), "OK ");
        }
        else
        {
            snprintf(indicator, sizeof(indicator), "ERR(%d) ", qsh_last_status);
        }
    }

    /* Build final prompt string */
    /* Example with colors:
       USER@HOST:CWD ( branch) HH:MM\nλ
    */
    const char *fmt_color =
        "%s" /* indicator */ COL_USERHOST "%s@%s" ANSI_RESET ":" COL_CWD "%s" ANSI_RESET " %s" COL_TIME "%s" ANSI_RESET "\n" COL_ARROW PRE_SIMBL " " ANSI_RESET;
    const char *fmt_no_color =
        "%s@%s:%s %s\n" PRE_SIMBL " ";

    char git_piece[128] = "";
    if (branch)
    {
        /* use small branch marker (branch symbol may not render on every font) */
        snprintf(git_piece, sizeof(git_piece), "( %s) ", branch);
    }

    /* Estimate needed size */
    size_t needed = 0;
    if (tty)
    {
        needed = strlen(fmt_color) + strlen(indicator) + strlen(user) + strlen(host) + strlen(cwd) + strlen(git_piece) + strlen(tbuf) + 100;
    }
    else
    {
        needed = strlen(fmt_no_color) + strlen(user) + strlen(host) + strlen(cwd) + strlen(git_piece) + strlen(tbuf) + 10;
    }

    char *out = malloc(needed);
    if (!out)
    {
        free(user);
        free(host);
        free(cwd);
        free(branch);
        return NULL;
    }

    if (tty)
    {
        /* include git piece in place of %s (we use COL_GIT before git_piece) */
        char colored_git[256] = "";
        if (branch)
        {
            snprintf(colored_git, sizeof(colored_git), COL_GIT "%s" ANSI_RESET, git_piece);
        }
        snprintf(out, needed, fmt_color, indicator, user, host, cwd, (branch ? colored_git : ""), tbuf);
        /* fmt_color expects 6 format tokens; using an empty prefix for alignment */
    }
    else
    {
        /* no color; attach git piece if present */
        char plain_git[128] = "";
        if (branch)
            snprintf(plain_git, sizeof(plain_git), "%s", git_piece);
        snprintf(out, needed, fmt_no_color, user, host, cwd, tbuf);
        /* append git piece before time if desired - keep simple for non-tty */
        if (branch)
        {
            /* insert git piece before time by reconstructing */
            char *tmp = malloc(strlen(user) + 1 + strlen(host) + 1 + strlen(cwd) + 1 + strlen(plain_git) + 1 + strlen(tbuf) + 10);
            if (tmp)
            {
                sprintf(tmp, "%s@%s:%s %s%s\nλ ", user, host, cwd, plain_git, tbuf);
                free(out);
                out = tmp;
            }
        }
    }

    /* cleanup */
    free(user);
    free(host);
    free(cwd);
    free(branch);
    return out;
}
