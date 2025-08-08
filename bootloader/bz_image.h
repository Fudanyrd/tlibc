#ifndef _BZ_IMAGE
#define _BZ_IMAGE

#ifdef __cplusplus
extern "C" {
#endif 

#include <stddef.h>

#include "serial.h"

// the sector size regardless of hardware.
#define LINUX_SECTOR_SIZE (512u)

#define LOADED_HIGH (1u << 0)

/**
 * This is used to define a more extensible 
 * boot parameters passing mechanism.
 */
struct setup_data {
  /**< 64-bit physical pointer to the next node of linked list,
   * 0 if last element in it. */
  uint64_t next;

  /**< identify the contents of data */
  uint32_t type;

  /**< length of the data. */
  uint32_t len;
  uint8_t data[];
};

struct KernelHeader {
  uint8_t unused[0x1f1];

  /** Modified from html source code. 01f1/1 stands for
   * offset 0x1f1, size 1 byte. */

  /* 01F1/1 */
  uint8_t setup_sects;
  /* The size of the setup in sectors. 
    If this field is 0, the real value is 4. 
    The real-mode code consists of the boot sector 
    (always one 512-byte sector) plus the setup code. */

  /* 01F2/2 */
  uint16_t root_flags;
  /* If set; the root is mounted readonly.
    If this field is nonzero, the root defaults to readonly. 
    The use of this field is deprecated; use the "ro" or "rw" 
    options on the command line instead. */

  /* 01F4/4 */
  uint32_t syssize;
  /* The size of the 32-bit code in 16-byte paras */

  /* 01F8/2 */
  uint16_t ram_size;
  /* DO NOT USE - for bootsect.S use only */

  /* 01FA/2 */
  uint16_t vid_mode;
  /* Video mode control */

  /* 01FC/2 */
  uint16_t root_dev;
  /* Default root device number
    The default root device device number. 
    The use of this field is deprecated, use the "root=" option 
    on the command line instead. */

  /* 01FE/2 */
  uint16_t boot_flag;
  /* 0xAA55 magic number */

  /* 0200/2 */
  uint16_t jump;
  /* Jump instruction
   Contains an x86 jump instruction, 0xEB followed by a signed 
   offset relative to byte 0x202. 
   This can be used to determine the size of the header. */

  /* 0202/4 */
  uint32_t header;
  /* Magic signature "HdrS" */

  /* 0206/2 */
  uint16_t version;
  /* Boot protocol version supported
   n (major << 8) + minor format, e.g. 0x0204 for version 2.04, 
   and 0x0a11 for a hypothetical version 10.17 */

  /* 0208/4 */
  uint32_t realmode_swtch;
  /* Boot loader hook (see below) */

  /* 020C/2 */
  uint16_t start_sys_seg;
  /* The load-low segment (0x1000) (obsolete) */

  /* 020E/2 */
  uint16_t kernel_version;
  /* Pointer to kernel version string */

  /* 0210/1 */
  uint8_t type_of_loader;
  /* Boot loader identifier. If does not have an
    assigned ID, enter 0xff here. */

  /* 0211/1 */
  uint8_t loadflags;
  /* Boot protocol option flags 
   Bit 0 (read): LOADED_HIGH
   -  If 0, the protected-mode code is loaded at 0x10000.
   -  If 1, the protected-mode code is loaded at 0x100000. 

   Bit 1 (kernel_internal)
   -  used internally by the compressed kernel to communicate 
      KASLR status to kernel proper.
   -  If 1, KASLR enabled.
   -  If 0, KASLR disabled.

   Bit 5 (write)
   -  if 0, print early messages.

   Bit 7 (write)
   -  if 1, can use heap.
   -  else, some setup functionality is disabled.
  */

  /* 0212/2 */
  uint16_t setup_move_size;
  /* Move to high memory size (used with hooks) */

  /* 0214/4 */
  uint32_t code32_start;
  /* Boot loader hook (see below) 
  This field can be modified for two purposes:

  1.  as a boot loader hook (see Advanced Boot Loader Hooks below.)
  2.  if a bootloader which does not install a hook loads a relocatable 
      kernel at a nonstandard address it will have to modify this 
      field to point to the load address. */

   /* 0218/4 */
  uint32_t ramdisk_image;
   /* initrd load address (set by boot loader) 
   (Write) The 32-bit linear address of the initial ramdisk or ramfs. 
   Leave at zero if there is no initial ramdisk/ramfs. */

  /* 021C/4 */
  uint32_t ramdisk_size;
  /* initrd size (set by boot loader)
  (write) Size of the initial ramdisk or ramfs. Leave at zero if 
  there is no initial ramdisk/ramfs. */

  /* 0220/4 */
  uint32_t bootsect_kludge;
  /* DO NOT USE - for bootsect.S use only */

  /* 0224/2 */
  uint16_t heap_end_ptr;
  /* Free memory after setup end 
  (write) Set this field to the offset (from the beginning of the real-mode code) 
  of the end of the setup stack/heap, minus 0x0200. */

  /* 0226/1 */
  uint8_t ext_loader_ver;
  /* Extended boot loader version 
  (write) The use of this field is boot loader specific. 
  If not written, it is zero.  */

  /* 0227/1 */
  uint8_t ext_loader_type;
  /* Extended boot loader ID 
  This field is ignored if the type in type_of_loader is not 0xE. */

  /* 0228/4 */
  uint32_t cmd_line_ptr;
  /* 32-bit pointer to the kernel command line (write)
   Set this field to the linear address of the kernel command line. 
   The kernel command line can be located anywhere between the end 
   of the setup heap and 0xA0000; it does not have to be located 
   in the same 64K segment as the real-mode code itself.

   Fill in this field even if your boot loader does not support 
   a command line, in which case you can point this to an empty 
   string (or better yet, to the string “auto”.) If this field 
   is left at zero, the kernel will assume that your boot loader 
   does not support the 2.02+ protocol. */

  /* 022C/4 */
  uint32_t initrd_addr_max;
  /* Highest legal initrd address 
  The maximum address that may be occupied by the initial 
  ramdisk/ramfs contents. */

  /* 0230/4 */
  uint32_t kernel_alignment;
  /* Physical addr alignment required for kernel (read/write)
  Alignment unit required by the kernel (if relocatable_kernel
  is true) */

  /* 0234/1 */
  uint8_t relocatable_kernel;
  /* Whether kernel is relocatable or not */

  /* 0235/1 */
  uint8_t min_alignment;
  /* Minimum alignment; as a power of two (read) */
  
  /* 0236/2 */
  uint16_t xloadflags;
  /* Boot protocol option flags(read) 
  Bit 0: If 1, this kernel has the legacy 64-bit entry point at 0x200
  Bit 1: If 1, kernel/boot_params/cmdline/ramdisk can be above 4G.
  Bit 2: If 1, the kernel supports the 32-bit EFI handoff entry point 
         given at handover_offset.
  Bit 3: If 1, the kernel supports the 64-bit EFI handoff entry point 
         given at handover_offset + 0x200.
  Bit 4: If 1, the kernel supports kexec EFI boot with EFI runtime support.
  */
  
  /* 0238/4 */
  uint32_t cmdline_size;
  /* Maximum size of the kernel command line 
  (read) The maximum size of the command line without the 
  terminating zero */
  
  /* 023C/4 */
  uint32_t hardware_subarch;
  /* Hardware subarchitecture.
  write, 0 is the default. */

  /* 0240/8 */
  uint64_t hardware_subarch_data;
  /* Subarchitecture-specific data
   read, do not modify. */

  /* 0248/4 */
  uint32_t payload_offset;
  /* Offset of kernel payload
   If non-zero then this field contains the offset 
   from the beginning of the protected-mode code to the payload.

   The payload may be compressed. The format of both the compressed and 
   uncompressed data should be determined using the standard magic numbers. 
   The uncompressed format is ELF. */
  
  /* 024C/4 */
  uint32_t payload_length;
  /* Length of kernel payload(read) */

  /* 0250/8 */
  uint64_t setup_data;
  /* 64-bit physical pointer to linked list of struct setup_data(write)
  The 64-bit physical pointer to NULL terminated single linked list 
  of struct setup_data.
  */

  /* 0258/8 */
  uint64_t pref_address;
  /* Preferred loading address */

  /* 0260/4 */
  uint32_t init_size;
  /* Linear memory required during initialization
  This field indicates the amount of linear contiguous memory starting at 
  the kernel runtime start address that the kernel needs before it is 
  capable of examining its memory map. This is not the same thing as the 
  total amount of memory the kernel needs to boot, but it can be used by 
  a relocating boot loader to help select a safe load address for the kernel.
  */

  /* 0264/4 */
  uint32_t handover_offset;
  /* Offset of handover entry point
  This field is the offset from the beginning of the kernel image to the 
  EFI handover protocol entry point. Boot loaders using the EFI handover 
  protocol to boot the kernel should jump to this offset. */

  /* 0268/4 */
  uint32_t kernel_info_offset;
  /* Offset of the kernel_info(read)
  This field is the offset from the beginning of the kernel image to the 
  kernel_info. The kernel_info structure is embedded in the Linux image in 
  the uncompressed protected mode region. */
  
  uint8_t unused2[2 * LINUX_SECTOR_SIZE - 0x26c];
} __attribute__((packed));

#define __bzimage_check \
  do { \
    __check_sizeof_int; \
    __static_assert(sizeof(struct KernelHeader) == LINUX_SECTOR_SIZE * 2); \
    __static_assert(offsetof(struct KernelHeader, boot_flag) == 0x1fe); \
    __static_assert(offsetof(struct KernelHeader, kernel_info_offset) == 0x268); \
  } while (0)


// compile with -O2 will optimize this away.
__attribute__((constructor))
static void bzimage_init(void) {
  __bzimage_check;
}

#ifdef __cplusplus
}
#endif // C++

#endif // _BZ_IMAGE
