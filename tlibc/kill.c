#include "std.h"

#define SIGKILL 9
#define SIGSEGV 11
#define SIGCHLD 17
#define SIGSTOP 19

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

int main(int argc, char **argv) {
    static const char *help = "Usage: kill [-h] or kill [-s signo] pid\n";
    if (argc < 2) {
        sys_write(2, help, 41);
        return 1;
    }

    int signo = SIGKILL;
    int pid = -1;
    bool h = false;

    // parse arguments
    for (int i = 1; i < argc; i++) {
        if (Strcmp(argv[i], "-h") == 0) {
            h = true;
            continue;
        }
        if (Strcmp(argv[i], "-s") == 0) {
            i++;
            signo = atoi(argv[i]);
            continue;
        }
        pid = atoi(argv[i]);
    }

    if (h) {
        sys_write(1, help, 41);
        return 0;
    }

    return sys_kill(pid, signo);
}
