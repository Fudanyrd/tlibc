#include "std.h"

/** Assume that path name length <= 14. */
#define MAXFILE 16

static char buf[32];

static char *ftype[] = {
    [DT_CHR] "character device",
    [DT_BLK] "block device",
    [DT_DIR] "directory",
    [DT_FIFO] "pipe",
    [DT_LNK] "symbolic link",
    [DT_REG] "file",
    [DT_UNKNOWN] "???",
    [DT_SOCK] "domain socket",
};

int main(int argc, char **argv) {
    int fd = sys_open(argc < 2 ? "." : argv[1], O_RDONLY | O_DIRECTORY);
    if (fd < 0) {
        sys_write(2, "Bad file descriptor\n", 21);
        sys_exit(1);
    }

    struct linux_dirent *di = (struct linux_dirent *)buf;
    long nread;

    while ((nread = sys_getdents(fd, di, sizeof(struct linux_dirent) + 16)) != 0) {
        if (nread < 0) {
            Puts("<too long> ??\n");
            sys_exit(1);
        }
        struct linux_dirent *it = di;
        for (; (char *)it < (char *)buf + nread; ) {
            const char *filename = (const char *)it;
            const char *typeaddr = filename + it->d_reclen - 1;
            filename += offsetof(struct linux_dirent, d_name);
            const char *typename = ftype[*typeaddr];
            Printf("%s %s\n", filename,
                   typename == NULL ? ftype[DT_UNKNOWN] : typename);
            it = (struct linux_dirent *)((uintptr_t)it + it->d_reclen);
        }
    }

    return 0;
}
