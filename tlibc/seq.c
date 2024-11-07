#include "std.h"

int main(int argc, char **argv) {
    int first = 1;
    int last = 0;
    switch (argc) {
        case 2: {
            last = atoi(argv[1]);
            break;
        }
        case 3: {
            first = atoi(argv[1]);
            last = atoi(argv[2]);
            break;
        }
        default: {
            Puts("usage: seq <first> <last> or seq <last>\n");
            sys_exit(1);
        }
    }

    for (int i = first; i <= last; i++) {
        Printf("%d\n", i);
    }
    return 0;
}
