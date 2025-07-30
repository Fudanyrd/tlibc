// Load the Linux kernel to address 0x100000.
// Parse its ELF headers, and jump to its start address.
//
// Specification:
// * The kernel to be loaded is 64-bit.
// * Address [0x1000, 0x1200] is reserved for sector data.
// * The program is put at address 0x1200
// * You need to install gcc-multilib to get this working.

#include <elf.h>

#include "serial.h"

/** Memory layout
              ~                        ~
              |  Protected-mode kernel |
100000        +------------------------+
              |  I/O memory hole       |
0A0000        +------------------------+
              |  Reserved for BIOS     |      Leave as much as possible unused
              ~                        ~
              |  Command line          |      (Can also be below the X+10000 mark)
X+10000       +------------------------+
              |  Stack/heap            |      For use by the kernel real-mode code.
X+08000       +------------------------+
              |  Kernel setup          |      The kernel real-mode code.
              |  Kernel boot sector    |      The kernel legacy boot sector.
X             +------------------------+
              |  Boot loader           |      <- Boot sector entry point 0000:7C00
001000        +------------------------+
              |  Reserved for MBR/BIOS |
000800        +------------------------+
              |  Typically used by MBR |
000600        +------------------------+
              |  BIOS use only         |
000000        +------------------------+

... where the address X is as low as the design of the boot loader permits. 

  Cite: https://kernel.org/doc/html/latest/arch/x86/boot.html

  This kernel loader should be put at address 0x1000.
  Also, assumes that the linux kernel is 64-bit.
 */

#define LINUX_LOAD_ADDR ((void *)0x100000)

static inline void __k_memzero(void *addr, uint32_t size) {
  uint8_t *buf = addr;
  for (uint32_t i = 0; i < size; i++) {
    buf[i] = 0;
  }
}

/**
 * @param fstart start sector of file
 * @param foff file offset
 * @param fsize number of bytes to read
 */
static inline void __k_readfile(void *addr, uint32_t fstart, uint32_t foff,
        uint32_t fsize) {
  
  unsigned char *sector_data = 0x1000;
  unsigned char *dst = addr;

  while (fsize != 0) {
    const uint32_t read_start = foff % SECTSIZE;
    uint32_t sector = fstart + foff / SECTSIZE;
    uint32_t nb = SECTSIZE - read_start;
    nb = nb > fsize ? fsize : nb;

    read_disk(sector_data, sector);

    for (uint32_t i = 0; i < nb; i++) {
      dst[i] = sector_data[i + read_start];
    }

    // advance.
    fsize -= nb;
    foff += nb;
    dst += nb;
  }
}

typedef void (*__entry_t)(void);

__attribute__((noreturn))
void _start(uint32_t sector) {
  // load first sector of kernel to the address.

  char data[512];
  read_disk(data, sector);

  Elf64_Ehdr *ehdr = (void *)data;
  __entry_t kernel_entry = ehdr->e_entry;
  uint16_t phnum = ehdr->e_phnum;
  uint32_t phoff = ehdr->e_phoff;

  char *sector_data = 0x1000;
  for (uint16_t i = 0; i < phnum; i++) {
    uint32_t off = phoff + i * sizeof(Elf64_Phdr);
    read_disk(data, sector + off / SECTSIZE);
    Elf64_Phdr *phdr = &data[off % SECTSIZE];

    if (phdr->p_type & PT_LOAD) {
      // load this into memory
      __k_readfile(phdr->p_paddr, sector, phdr->p_offset, phdr->p_filesz);
      
      // set the rest to zero.
      __k_memzero(phdr->p_paddr + phdr->p_filesz, phdr->p_memsz - phdr->p_filesz);
    }
  }

  // jump to the kernel.
  kernel_entry();

  // should not reach here.
  for (;;) { }
}
