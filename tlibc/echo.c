#include "std.h"

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        Printf("%s ", argv[i]);
    }

    sys_write(1, "\n", 1);
    return 0;
}
