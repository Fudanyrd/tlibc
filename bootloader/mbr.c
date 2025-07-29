// Usage: compile it in 16-bit mode via:
// gcc -m16 -c -fpic -Os
// extract the .text, and put the code
// in the first 440 bytes of MBR sector.
#include "serial.h"

void _start() {
  void *addr = 0x7a00;
  read_disk(addr, 0);

  putch('r');
  putch('e');
  putch('a');
  putch('d');
  putch('y');
  putch('\n');

  asm volatile("hlt" : : : "memory");
  for (;;) {}
}

