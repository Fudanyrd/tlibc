
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
