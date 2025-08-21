#include "std.h"

int main(int argc, char **argv, char **envp) {
    for (int i = 0; envp[i]; i++) {
        Printf("%s\n", envp[i]);
    }

    return 0;
}

