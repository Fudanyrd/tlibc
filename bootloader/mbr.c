// Usage: compile it in 16-bit mode via:
// gcc -m16 -c -fpic -Os
// extract the .text, and put the code
// in the first 440 bytes of MBR sector.
#include "serial.h"

void _start() {
  __check_sizeof_int;

  putch('E');
  read_disk(0x1200, 3 + 528);
  read_disk(0x1200 + SECTSIZE, 3 + 528 + 1);

  typedef void (*fn)(uint32_t);
  fn secondary = 0x1200;

  putch('S');
  putch('\n');

  // start secondary boot loader,
  // i.e. vmlinux(a tiny ELF loader)
  secondary(3 + 536);

  asm volatile("hlt");
  for(;;) {}
}

