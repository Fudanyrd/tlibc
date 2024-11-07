#include "sys.h"

int main(int argc, char **argv) {
    sys_write(1, "Crash!\n", 7);
    *(int *)0x0 = 0;
    return 0;
}
