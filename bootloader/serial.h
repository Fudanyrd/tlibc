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
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
#endif // _ELF_H

/**<  Volume 3, Figure 3-8, load segment descriptor with `lgdt` 
 * 
 * `Base Address`: Defines the location of byte 0 of the segment 
 * within the 4-GByte linear address space. The  processor puts 
 * together the three base address fields to form a single 32-bit 
 * value. Segment base addresses should be aligned to 16-byte boundaries. 
 */
struct SegmentDescriptor {
  /**< segment limit [15:0] */
  uint32_t seg_limit_low : 16;
  /**< base address [15:0] */
  uint32_t base_low : 16;

  /**< base [23:16] */
  uint32_t base_mid : 8;
  /**< segment type */
  uint32_t type : 4;
  /**< Descriptor type(0-system, 1-code or data). */
  uint32_t S : 1;
  /**< descriptor previledge level */
  uint32_t DPL : 2;
  /**< presence */
  uint32_t P : 1;
  /**< segment limit [19:16] */
  uint32_t seg_limit_high : 4;
  /**< available for use of system software */
  uint32_t AVL : 1;
  /**< 64-bit code segment
     If L = 1 and D = 0, run in 64-bit mode.
     If D = 1, set operand size as 32-bit. */
  uint32_t L : 1;
  /**< Default operation size, 0: 16 bit, 1: 32bit */
  uint32_t DB : 1;
  /**< granularity, when flag is set, the segment 
    limit is interpreted in 4-KByte units */
  uint32_t G : 1;
  /**< base address [31:24] */
  uint32_t base_high : 8;
} __attribute__((packed));

/**< Layout of a segment selector(cs, ds, ss, etc.)
 * To load a value into cs, you should do: mov $(val << 3), %cs */
struct SegmentSelector {
  /**< request privilege level */
  uint16_t RPL : 2;
  /**< Table indicator, 0: global, 1: local */
  uint16_t TI : 1;
  /**< bits [3:15] */
  uint16_t Index : 13;
} __attribute__((packed));

/**< Layout of 32-bit Descriptor Table.
   See `lgdt` instruction in volume 2A. */
struct DescriptorTable32 {
  /**< (.word) size of table in bytes */
  uint16_t limit;
  /**< (.long) start address of the table. */
  uint32_t ptr;
} __attribute__((packed));

/**< Layout of 64-bit Descriptor Table.
   See `lgdt` instruction in volume 2A. */
struct DescriptorTable64 {
  /**< (.word) size of table in bytes */
  uint16_t limit;
  /**< (.quad) start address of the table. */
  uint64_t ptr;
} __attribute__((packed));

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

// control linkage type of read_disk
#ifdef __EXTERN_READ_DISK
void read_disk(void *buf, int sect) 
#else
static inline void read_disk(void *buf, int sect) 
#endif // __EXTERN_READ_DISK
{
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

  // this ensures that size of integer are expected.
#define __check_sizeof_int \
  do {\
    __static_assert(sizeof(uint8_t) == 1); \
    __static_assert(sizeof(uint16_t) == 2); \
    __static_assert(sizeof(uint32_t) == 4); \
    __static_assert(sizeof(uint64_t) == 8); \
    __static_assert(sizeof(int8_t) == 1); \
    __static_assert(sizeof(int16_t) == 2); \
    __static_assert(sizeof(int32_t) == 4); \
    __static_assert(sizeof(int64_t) == 8); \
    __static_assert(sizeof(struct SegmentDescriptor) == 8); \
    __static_assert(sizeof(struct SegmentSelector) == 2); \
    __static_assert(sizeof(struct DescriptorTable32) == 6); \
    __static_assert(sizeof(struct DescriptorTable64) == 10); \
  } while (0)

#endif // __static_assert

#endif // __SERIAL_H

