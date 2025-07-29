// extract .text section from ELF executable.
// use for making mbr sectors, bootloaders, etc.
// Usage: gettext [Input ELF] [Output Image]

#include <elf.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void usage() {
  printf("usage: gettext [Input ELF] [Output Image]\n");
  return;
}

static void xreadat(int fd, void *buf, off_t offset, size_t count) {
  if (lseek(fd, offset, SEEK_SET) != offset) {
    perror("seek");
    _exit(1);
  }

  if (read(fd, buf, count) != count) {
    perror("read");
    _exit(1);
  }

  // OK.
}

static void xfcopy(int srcFd, int dstFd, size_t count, off_t offset) {
  static unsigned char buf[512];

  while (count) {
    size_t nr = count > sizeof(buf) ? sizeof(buf) : count;
    xreadat(srcFd, buf, offset, nr);

    count -= nr;
    offset += nr;

    if (write(dstFd, buf, nr) != nr) {
      perror("write");
      _exit(1);
    }
  }

  // OK
}

int main(int argc, char **argv) {

  if (argc != 3) {
    usage();
    return 1;
  }

  int srcFd = open(argv[1], O_RDONLY);
  if (srcFd < 0) {
    perror("open src");
    return 1;
  }
  int dstFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0777);
  if (dstFd < 0) {
    perror("open dst");
    return 1;
  }

  static Elf32_Ehdr e32;
  static Elf64_Ehdr e64;
  bool m64 = false;
  xreadat(srcFd, &e32, 0, sizeof(e32));
  if (e32.e_machine == EM_X86_64) {
    m64 = true;
    xreadat(srcFd, &e64, 0, sizeof(e64));
  }

  size_t phnum;
  off_t phoff;
  if (m64) {
    // read 64-bit executable
    static Elf64_Phdr p64;

    phnum = e64.e_phnum;
    phoff = e64.e_phoff;

    for (size_t i = 0; i < phnum; i++) {
      xreadat(srcFd, &p64, phoff + sizeof(p64) * i, sizeof(p64));

      if ((p64.p_flags & PF_X) && p64.p_type == PT_LOAD) {
        xfcopy(srcFd, dstFd, p64.p_filesz, p64.p_offset);
      }
    }

  } else {
    static Elf32_Phdr p32;

    phnum = e32.e_phnum;
    phoff = e32.e_phoff;

    for (size_t i = 0; i < phnum; i++) {
      xreadat(srcFd, &p32, phoff + sizeof(p32) * i, sizeof(p32));

      if ((p32.p_flags & PF_X) && p32.p_type == PT_LOAD) {
        xfcopy(srcFd, dstFd, p32.p_filesz, p32.p_offset);
      }
    }
  }

  close(srcFd);
  close(dstFd);
  return 0;
}
