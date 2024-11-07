#include "std.h"

int main(int argc, char **argv) {
    static char buf[4096];

    // read from stdin
    if (argc < 2) {
        // read from stdin
        long ret;
        while ((ret = sys_read(0, buf, sizeof(buf))) > 0) {
            sys_write(1, buf, ret);
        }

        return 0;
    }

    // read from file specified in argv[1]
    int fd = sys_open(argv[1], O_RDONLY);
    if (fd < 0) {
        sys_exit(1);
    }
    
    // get the size of the file
    size_t fsz = sys_lseek(fd, 0, SEEK_END);
    sys_lseek(fd, 0, SEEK_SET);

    // map the file into memory
    // mmap(NULL, fsz, 0x1, 0x02, fd, 0);
    const char *dat = sys_mmap(NULL, fsz, PROT_READ, MAP_PRIVATE, fd, 0);
    if (dat == (void *)(-1) || ((uintptr_t)dat & 0xfff) != 0) {
        sys_write(2, "Bad mmap, abort\n", 16);
        return 1;
    }

    // write the file to stdout
    sys_write(1, dat, fsz);
    uintptr_t pt = (uintptr_t)dat;
    sys_munmap((char *)pt, fsz);
    sys_close(fd);
    return 0;
}
