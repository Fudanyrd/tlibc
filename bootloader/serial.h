// Credit to Yanyan Jiang
// This file is partly derived from the Abstract Machine project 
// from NJU.
// Git Repo <https://github.com/NJU-ProjectN/os-workbench-2022.git>

#ifndef __SERIAL_H
#define __SERIAL_H

#define SECTSIZE 512

#ifndef _ELF_H
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
#endif // _ELF_H

static inline uint8_t inb(int port) {
  uint8_t data;
  asm volatile ("inb %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline uint16_t inw(int port) {
  uint16_t data;
  asm volatile ("inw %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline uint32_t inl(int port) {
  uint32_t data;
  asm volatile ("inl %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline void outb(int port, uint8_t data) {
  asm volatile ("outb %%al, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void outw(int port, uint16_t data) {
  asm volatile ("outw %%ax, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void outl(int port, uint32_t data) {
  asm volatile ("outl %%eax, %%dx" : : "a"(data), "d"((uint16_t)port));
}

static inline void putch(char ch) {
  #define COM1 0x3f8
  outb(COM1, ch);
}

static inline void wait_disk(void) {
  while ((inb(0x1f7) & 0xc0) != 0x40);
}

static inline void read_disk(void *buf, int sect) {
  wait_disk();
  outb(0x1f2, 1);
  outb(0x1f3, sect);
  outb(0x1f4, sect >> 8);
  outb(0x1f5, sect >> 16);
  outb(0x1f6, (sect >> 24) | 0xE0);
  outb(0x1f7, 0x20);
  wait_disk();
  for (int i = 0; i < SECTSIZE / 4; i ++) {
    ((uint32_t *)buf)[i] = inl(0x1f0);
  }
}

#ifndef __static_assert
#define __static_assert(cond) \
  do {   \
    switch (0) { \
      case (0): break; \
      case (cond): break; \
    }; \
  } while (0)

#endif // __static_assert

#endif // __SERIAL_H

