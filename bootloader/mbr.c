// Usage: compile it in 16-bit mode via:
// gcc -m16 -c -fpic -Os
// extract the .text, and put the code
// in the first 440 bytes of MBR sector.
#include "serial.h"

void _start() {
  // this address is reserved for boot loader(our program)
  void *addr = 0x1000;
  read_disk(addr, 0);

  putch('r');
  putch('e');
  putch('a');
  putch('d');
  putch('y');
  putch('\n');

  register uint16_t memSz asm("ax");

  // use bios interrupt to determine
  // size of low memory available.
  // cite: https://kernel.org/doc/html/latest/arch/x86/boot.html
  // 
  // FIXME: the mov $0x0, %eax 
  // is possibly not needed because interrupt 0x12
  // takes no input.
  asm volatile("mov $0x0, %%eax\n\t"
    "int $0x12" : "=r"(memSz) :: );
  for (;;) {}
}

