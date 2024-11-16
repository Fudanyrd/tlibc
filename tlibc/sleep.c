#include "std.h"

int main(int argc, char **argv) {
    static struct timespec req = { 0, 0 };
    static struct timespec rem = { 0, 0 };

    if (argc < 2) {
        sys_write(2, "Usage: sleep <seconds>\n", 24);
        return 1;
    }

    req.tv_sec = atoi(argv[1]);
    sys_nanosleep(&req, &rem);
    return 0;
}
