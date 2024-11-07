#include "std.h"

static int stat(const char *fname) {
    static struct stat st;
    int fd = sys_open(fname, O_RDONLY);
    if (fd < 0) {
        sys_write(2, "cannot open ", 12);
        sys_write(2, fname, Strlen(fname));
        sys_write(2, "\n", 1);
        return 1;
    }

    // stat the file
    if (sys_fstat(fd, &st) != 0) {
        sys_write(2, "stat error\n", 11);
        return 1;
    }

    // print status
    Printf("%s: size %L, inode %L\n", fname, st.st_size, st.st_ino);

    sys_close(fd);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        sys_write(2, "usage: stat <file list>\n", 24);
        sys_exit(1);
    }

    int code = 0;
    for (int i = 1; i < argc; i++) {
        code |= stat(argv[i]);
    }
    sys_exit(code);
}
