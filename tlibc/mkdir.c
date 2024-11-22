#include "sys.h"
#include "fcntl.h"
#include <stdbool.h>

/** Make directory recursively. */
int mkdir(const char *dir) {
    static char buf[256];
    int i = 0;
    int ret = 0;
    bool is_end = true;

    // mkdir from root
    if (*dir == '/') {
        buf[i] = *dir;
        i++;
    }

    while (dir[i] != 0) {
        buf[i] = dir[i];
        if (dir[i] == '/') {
            buf[i] = 0;
            int fd = sys_open(buf, O_RDONLY);
            if (fd < 0) {
                // create this directory
                ret |= sys_mkdir(buf, 0777);
            } else {
                sys_close(fd);
            }
            buf[i] = '/';
            is_end = true;
        } else {
            is_end = false;
        }
        i++;
    }

    // make dir at last level
    if (!is_end) {
        int fd = sys_open(buf, O_RDONLY);
        if (fd < 0) {
            ret |= sys_mkdir(buf, 0777);
        } else {
            sys_close(fd);
        }
    }
    return ret == 0 ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        sys_write(2, "Usage mkdir <dir name>\n", 24);
        sys_exit(1);
    }

    int ret = 0;

    for (int i = 1; i < argc; i++) {
        ret |= mkdir(argv[i]);
    }
    return ret;
}
