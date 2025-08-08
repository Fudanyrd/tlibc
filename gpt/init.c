// use this as the init program after the kernel is loaded.
#define SYS_execve 59

static void execve(const char *filename, char *const argv[], char *const envp[]) {
  asm volatile (
    "mov %0, %%rdi\n"
    "mov %1, %%rsi\n"
    "mov %2, %%rdx\n"
    "mov $59, %%rax\n"
    "syscall\n"
    :
    : "r"(filename), "r"(argv), "r"(envp)
    : "%rax", "%rdi", "%rsi", "%rdx", "memory"
  );
}

void _start() {
  char *const argv[] = {"/busybox", "sh", (void *)0};
  char *const envp[] = {
    "PATH=/:/bin:/usr/bin",
    "TERM=linux",
    "HOME=/",
    "HOSTTYPE=x86_64",
    "PWD=/",
    (void *)0
  };

  execve(argv[0], argv, envp);
  asm volatile (
    "mov $60, %%rax\n"  // syscall number for exit
    "mov $1, %%rdi\n" // exit code 1
    "syscall\n"
    :
    :
    : "%rax", "%rdi", "memory"
  );
}
