#include "sys.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        sys_write(2, "Usage mkdir <dir name>\n", 24);
        sys_exit(1);
    }

    for (int i = 1; i < argc; i++) {
        sys_mkdir(argv[i], 0777);
    }
    sys_exit(0);
}
