#include "sys.h"
#include "std.h"

int main(int argc, char **argv) {
    // eg2();
    static struct buffered_reader br;
    static char buf[2048];
    br.start = br.end = 0;

    sys_write(1, "(yrd) ", 6);
    while (fdgets(&br, buf, 0)) {
        system(buf);
        sys_write(1, "(yrd) ", 6);
    }
    return 0;
}

/* Return true if ch is in string s. */
static bool contains(const char *s, char ch) {
    if (ch == '\0') {
        return true;
    }
    for (int i = 0; s[i]; i++) {
        if (s[i] == ch) {
            return true;
        }
    }
    return false;
}

static int Strcmp(const char *s1, const char *s2) {
    while (*s1 != 0 && *s2 != 0) {
        if (*s1 != *s2) {
            int ret = 0;
            ret += (int)*s1;
            ret -= (int)*s2;
            return ret;
        }
        s1 ++;
        s2 ++;
    }

    return (int)*s1 - (int)*s2;
}

static int system_single(const char *cmd, int end) {
    static job_t jobs[4];
    if (*cmd == 0) {
        return 0;
    }
    Memset(jobs, 0, sizeof(jobs));

    static const char *blank = " \t\r\n";
    int jobcnt = 1;
    int narg = 0;
    job_t *cur = (job_t *)jobs;
    char *cp = (char *)cur->buf;
    // is next token stdin, stdout or stderr?
    bool prein = false;
    bool preout = false;
    bool preerr = false;

    for (int i = 0; i < end; ) {
        int next = i;
        // -- search and record begin of token -- //
        for (; next < end; next++) {
            if (!contains(blank, cmd[next])) {
                break;
            }
        }
        i = next;

        // -- search end of token -- //
        for (next = i; next < end; next++) {
            if (contains(blank, cmd[next])) {
                break;
            }
        }

        // -- record in buffer -- //
        if (next > i) {
            char *old = cp;
            bool is_argv = true;
            for (int k = i; k < next; k++) {
                *cp = cmd[k];
                cp++;
            }
            *cp = 0;
            cp++;

            if (Strcmp(old, "|") == 0) {
                // a pipe!
                cur->pipe = false;
                // turn to next job
                is_argv = false;
                jobcnt++;
                cur++;
                narg = 0;
            }

            // current token is dest of redirection
            if (prein) {
                prein = false;
                is_argv = false;
                cur->stdin_fo = old;
            }
            if (preout) {
                preout = false;
                is_argv = false;
                cur->stdout_fo = old;
            }
            if (preerr) {
                preerr = false;
                is_argv = false;
                cur->stderr_fo = old;
            }
            if (Strcmp(old, "<") == 0) {
                prein = true;
                is_argv = false;
            }
            if (Strcmp(old, ">") == 0) {
                preout = true;
                is_argv = false;
            }
            if (Strcmp(old, "2>") == 0) {
                preerr = true;
                is_argv = false;
            }
            
            // is an argument
            if (is_argv) {
                if (narg == 0) {
                    cur->exe = old;
                }
                cur->argv[narg++] = old;
            }
        }

        // -- end loop -- //
        i = next;
    }

    // built in commands: cd, exit(q)
    if (Strcmp("cd", jobs[0].exe) == 0) {
        if (sys_chdir(jobs[0].argv[1]) != 0) {
            sys_write(2, "cd failure\n", 11);
        }
        return 0;
    }
    if (Strcmp("exit", jobs[0].exe) == 0 || 
        Strcmp("q", jobs[0].exe) == 0) {
        sys_write(1, "Bye.\n", 5);
        sys_exit(0);
        return 0;
    }

    return exec_job((job_t *)jobs, jobcnt);
}

// implementation of system
int system(const char *cmd) {
    // end of cmd chars
    static const char *eoc = "\n;&\0";
    int it = 0;
    int next = 0;
    int ret = 0;

    while (cmd[it]) {
        for (next = it; !contains(eoc, cmd[next]); next++) {}
        ret |= system_single(cmd + it, next - it);
        it = cmd[next] ? next + 1 : next;
    }

    return ret;
}
