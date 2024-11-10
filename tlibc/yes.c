#include "sys.h"

int main(int argc, char **argv) {
    for (;;) {
        sys_write(1, "y\n", 2);
    }

    sys_exit(0);
}
