#include "std.h"
#define BUFSZ 4096

void wc(int fd) {
    uint32_t nbyte = 0;
    uint32_t nword = 0;
    uint32_t nline = 0;

    /** Indicates type of characters. */
    static uint8_t chtype[256];

    /* Alphabet */
    for (int i = 'a'; i <= 'z'; i++) {
        chtype[i] = 1;
    }
    for (int i = 'A'; i <= 'Z'; i++) {
        chtype[i] = 1;
    }

    /* Digits */
    for (int i = '0'; i <= '9'; i++) {
        chtype[i] = 1;
    }

    /* Miscellaneous */
    for (const char *s = "~`!@#$%^&*()-_[]{}\"\'':<>,.\\/?"; *s; s++) {
        chtype[*s] = 1;
    }

    /* Is current character white? */
    bool white = true;
    bool end = false;

    /** Read 4096 bytes a time. */
    static char pool[BUFSZ];
    long nread = sys_read(fd, pool, sizeof(pool));
    if (nread < 0) {
        sys_exit(1);
    }
    if (nread == 0) {
        end = true;
    }

    while (nread > 0) {
        for (long i = 0; i < nread; i++) {

            // line
            if (pool[i] == '\n') {
                nline++;
                end = true;
            }

            // word
            if (white) {
                if (chtype[pool[i]]) {
                    white = false;
                    nword++;
                }
            } else {
                if (!chtype[pool[i]]) {
                    white = true;
                }
            }
        }
        nbyte += nread;
        nread = sys_read(fd, pool, sizeof(pool));
    }

    if (!end) {
        nline++;
    }

    Printf("\t%u\t%u\t%u\n", nline, nword, nbyte);
    return;
}

int main(int argc, char **argv) {
    int fd = argc < 2 ? 0 : sys_open(argv[1], O_RDONLY);
    if (fd < 0) {
        Puts("Invalid fd.\n");
        sys_exit(1);
    }
    wc(fd);

    sys_close(fd);
    return 0;
}
