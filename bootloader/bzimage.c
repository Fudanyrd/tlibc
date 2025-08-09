// A simple bzImage loader(used with bzimage.S).
// currently supports boot protocol version >= 2.02


// clang-format off
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

... where X = 0x2000. Therfore, cs = 0x220, 
  ds = es = fs = gs = ss = 0x200 before jumping to the setup code.

  This kernel loader is put at address 0x1200.
 */
// clang-format on

// do not inline read_disk.
#define __EXTERN_READ_DISK

#include "bz_image.h"
#include "serial.h"

__attribute__((section(".text")))
static const char *LINUX_CMD_LINE = "init=/init trace_clock=local console=ttyS0 earlyprintk root=/dev/sda1 rw";

// jump to the setup code.
extern void jump_to_setup(void *base_ptr, void *heap_end);

// halt the CPU
extern void halt() __attribute__((noreturn));

static void die() __attribute__((noreturn));

__attribute__((section(".text.str")))
static const uint8_t code16[] = {
  0x31, 0xc0, // xor ax, ax
  0x05, 0x00, 0x02, // add ax, 0x200
  0x8e, 0xd8, // mov ds, ax
  0x8e, 0xd0, // mov ss, ax
  0x8e, 0xc0, // mov es, ax
  0x8e, 0xe0, // mov fs, ax
  0x8e, 0xe8, // mov gs, ax
  0x31, 0xc0, // xor ax, ax
  0xff, 0xe0, // jmp *%ax
};

__attribute__((section(".text.code")))
static const char *message = "DIE!";

void __elf_main() {
  // read the boot sector and real-mode code.
  
  uint32_t sector = 539 /** FIXME: hard-coded start sector of bzImage. */;
  void *load_start = 0x2000;
  read_disk(load_start, sector);
  read_disk(load_start + LINUX_SECTOR_SIZE, sector + 1);

  // check the kernel header.
  struct KernelHeader *khdr = load_start;
  uint8_t setup_sects = khdr->setup_sects;
  setup_sects = setup_sects == 0 ? 4 : setup_sects;
  setup_sects ++;
  if (khdr->boot_flag != 0xaa55 || khdr->header != 0x53726448) {
    // this means the location of bzImage is wrong.
    // double-check this.
    die();
  }

  // read the rest of real-mode code.
  void *load_addr = load_start + (LINUX_SECTOR_SIZE * 2);
  for (uint8_t i = 2; i < setup_sects; i++) {
    read_disk(load_addr, sector + i);
    load_addr += LINUX_SECTOR_SIZE;
  }

  // read the protected-mode kernel.
  const uint32_t img_size = (setup_sects * LINUX_SECTOR_SIZE) + (khdr->syssize * 16);
  const uint16_t protocol = khdr->version;
  if (!(khdr->loadflags & LOADED_HIGH) || protocol < 0x0202) {
    // not supported.
    die();
  }
  load_addr = 0x100000;
  for (uint32_t i = setup_sects; i < (img_size / LINUX_SECTOR_SIZE); i++) {
    read_disk(load_addr, sector + i);
    load_addr += LINUX_SECTOR_SIZE;
  }

  khdr->type_of_loader = 0xFF; // set the type of loader to 0xFF.
  uint32_t heap_end;
  heap_end = 0x10000;
  khdr->heap_end_ptr = heap_end - 0x200;
  khdr->loadflags |= 0x80; // set the heap flag.

  do {
    char *cmd_line = (const char *)(load_start + heap_end);
    for (const char *pt = LINUX_CMD_LINE; *pt; pt++) {
      *cmd_line++ = *pt;
    }
    *cmd_line = '\0'; // null-terminate the command line.
    khdr->cmd_line_ptr = (uint32_t)(load_start + heap_end);
  } while (0);

  /** Put the 16-bit code at the start of the heap. */
  uint8_t *code16_start = (uint8_t *)load_start + 0x8000;
  for (uint8_t i = 0; i < sizeof(code16); i++) {
    code16_start[i] = code16[i];
  }

#ifndef __HALT_AFTER_LOAD
  // jump to real mode code to start the kernel.
  jump_to_setup(load_start, heap_end);
#endif // __HALT_AFTER_LOAD

  // should not reach here because kernel should be running.
  die();
}

static void die() {
  for (const char *pt = message; *pt; pt++) {
    putch (*pt);
  }

  halt();
}
