// A simple bzImage loader(used with bzimage.S).

// do not inline read_disk.
#define __EXTERN_READ_DISK

#include "bz_image.h"
#include "serial.h"

// jump to the setup code.
extern void jump_to_setup(void *base_ptr, void *heap_end);

// halt the CPU
extern void halt() __attribute__((noreturn));

static void die() __attribute__((noreturn));

void __elf_main() {
  // read the boot sector and real-mode code.
  
  uint32_t sector = 539 /** FIXME: hard-coded start sector. */;
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
  /** FIXME: the size of bzImage(in bytes) is hard-coded */
  const uint32_t img_size = 18019328;
  load_addr = 0x100000;
  for (uint32_t i = setup_sects; i < (img_size / LINUX_SECTOR_SIZE); i++) {
    read_disk(load_addr, sector + i);
    load_addr += LINUX_SECTOR_SIZE;
  }

  // jump to real mode code to start the kernel.
  jump_to_setup(load_start, 0x9000);

  // should not reach here because kernel should be running.
  die();
}

__attribute__((section(".text")))
static const char *message = "DIE!";

static void die() {
  for (const char *pt = message; *pt; pt++) {
    putch (*pt);
  }

  halt();
}
