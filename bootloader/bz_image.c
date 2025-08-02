// routine for loading bzImage
// i.e. the output of `file` program should look like:
//
// Linux kernel x86 boot executable bzImage, 
//// version 6.12.40 (root@19bca55c7ef6) 
//// #5 SMP PREEMPT_DYNAMIC Fri Aug  1 01:13:16 UTC 2025, 
//// RO-rootFS, swap_dev 0X11, Normal VGA

#include <stddef.h>

#include "serial.h"

// the sector size regardless of hardware.
#define LINUX_SECTOR_SIZE (512u)

struct KernelHeader {
  uint8_t unused[0x1f1];

  /** Modified from html source code. 01f1/1 stands for
   * offset 0x1f1, size 1 byte. */

  /* 01F1/1 */
  uint8_t setup_sects;
  /* The size of the setup in sectors */

  /* 01F2/2 */
  uint16_t root_flags;
  /* If set; the root is mounted readonly */

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
  /* Default root device number */

  /* 01FE/2 */
  uint16_t boot_flag;
  /* 0xAA55 magic number */

  /* 0200/2 */
  uint16_t jump;
  /* Jump instruction */

  /* 0202/4 */
  uint32_t header;
  /* Magic signature “HdrS” */

  /* 0206/2 */
  uint16_t version;
  /* Boot protocol version supported */

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
  /* Boot loader identifier */

  /* 0211/1 */
  uint8_t loadflags;
  /* Boot protocol option flags */

  /* 0212/2 */
  uint16_t setup_move_size;
  /* Move to high memory size (used with hooks) */

  /* 0214/4 */
  uint32_t code32_start;
  /* Boot loader hook (see below) */

   /* 0218/4 */
  uint32_t ramdisk_image;
   /* initrd load address (set by boot loader) */

  /* 021C/4 */
  uint32_t ramdisk_size;
  /* initrd size (set by boot loader) */

  /* 0220/4 */
  uint32_t bootsect_kludge;
  /* DO NOT USE - for bootsect.S use only */

  /* 0224/2 */
  uint16_t heap_end_ptr;
  /* Free memory after setup end */

  /* 0226/1 */
  uint8_t ext_loader_ver;
  /* Extended boot loader version */

  /* 0227/1 */
  uint8_t ext_loader_type;
  /* Extended boot loader ID */

  /* 0228/4 */
  uint32_t cmd_line_ptr;
  /* 32-bit pointer to the kernel command line */

  /* 022C/4 */
  uint32_t initrd_addr_max;
  /* Highest legal initrd address */

  /* 0230/4 */
  uint32_t kernel_alignment;
  /* Physical addr alignment required for kernel */

  /* 0234/1 */
  uint8_t relocatable_kernel;
  /* Whether kernel is relocatable or not */

  /* 0235/1 */
  uint8_t min_alignment;
  /* Minimum alignment; as a power of two */
  
  /* 0236/2 */
  uint16_t xloadflags;
  /* Boot protocol option flags */
  
  /* 0238/4 */
  uint32_t cmdline_size;
  /* Maximum size of the kernel command line */
  
  /* 023C/4 */
  uint32_t hardware_subarch;
  /* Hardware subarchitecture */

  /* 0240/8 */
  uint64_t hardware_subarch_data;
  /* Subarchitecture-specific data */

  /* 0248/4 */
  uint32_t payload_offset;
  /* Offset of kernel payload */
  
  /* 024C/4 */
  uint32_t payload_length;
  /* Length of kernel payload */

  /* 0250/8 */
  uint64_t setup_data;
  /* 64-bit physical pointer to linked list of struct setup_data */

  /* 0258/8 */
  uint64_t pref_address;
  /* Preferred loading address */

  /* 0260/4 */
  uint32_t init_size;
  /* Linear memory required during initialization */

  /* 0264/4 */
  uint32_t handover_offset;
  /* Offset of handover entry point */

  /* 0268/4 */
  uint32_t kernel_info_offset;
  /* Offset of the kernel_info */
  
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
