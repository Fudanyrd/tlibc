#include "sys.h"
#include "std.h"

int main(int argc, char **argv) {
    static char buf[4096];
    sys_getcwd(buf, sizeof(buf));
    sys_write(1, buf, Strlen(buf));
    sys_write(1, "\n", 1);
    return 0;
}
