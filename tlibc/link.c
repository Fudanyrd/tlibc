#include "sys.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        sys_write(2, "Usage: link <oldpath> <newpath>\n", 33);
        sys_exit(1);
    }

    int exitcode = 0;
    if (sys_link(argv[1], argv[2])) {
        sys_write(2, "link failure\n", 14);
        exitcode++;
    }
    sys_exit(exitcode);
}
