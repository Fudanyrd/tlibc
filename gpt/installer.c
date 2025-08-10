// Install boot sector and linux bzImage into a 
// block device. After this operation, the block
// device can be booted by hardware.
//
// This installer do the following:
// * Create a GPT partition table on it;
// * Create a FAT32 file system on the first partition;
// * Copy the boot code into MBR boot sector(<= 440 KByte);
// * Copy the bzImage and secondary bootloader(bootloader/bzimage)
//
// Warning: This will erase all data on the block
// device!

#include "fat.h"
#include "gpt.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void *buf = NULL;
static size_t disk_size = 0;

__attribute__((noreturn))
static void die(const char *err) {
  if (buf) {
    munmap(buf, disk_size);
  }
  if (err) {
    perror(err); 
  }
  _exit(1);
}

static const char *basename(const char *path) {
  const char *ret = path;
  for (const char *pt = path; *pt; pt++) {
    if (*pt == '/' ) {
      ret = pt;
    }
  }
  return ret;
}

static size_t fdsize(int fd, const char* path) {
  struct stat st;

  if (fstat(fd, &st) != 0) {
    die("fstat");
  }

  // check it is a regular file or a block device.
  // see: man 7 inode

#ifndef S_IFREG
#define S_IFREG    0100000 //   regular file
#define S_IFBLK    0060000 //   block device
#define S_IFDIR    0040000 //   directory
#define S_IFCHR    0020000 //   character device
#define S_IFIFO    0010000 //   FIFO
#endif
  
  const mode_t mode = st.st_mode;
  if ((mode & S_IFREG) == S_IFREG) {
    return st.st_size;
  }

  if (S_ISBLK(mode)) {
    char sbuf[192];
    sprintf(sbuf, "/sys/class/block/%s/size", basename(path));
    FILE *fobj = fopen(sbuf, "r");
    if (fobj == NULL) {
       die("fopen");
    }
    size_t ret = 0;
    fscanf(fobj, "%ld", &ret);
    fclose(fobj);
    return ret * 512;
  }

  // not a regular file or block device.
  fprintf(stderr, "Not a file or block dev\n");
  _exit(1);
}


static struct PartitionConfig pc[2];  
static struct GPTConfig config;
static void mkpart(size_t boot_size) {
// C12A7328-F81F-11D2-BA4B-00A0C93EC93B
static const struct GUID EFI_SYSTEM_PARTITION = {
  0xc12a7328,
  0xf81f,
  0x11d2,
  0xba, 0x4b,
  {0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b},
};

// 9E8D8D40-E4D1-3547-AD9E-86523F5E2F4A
static const struct GUID myDiskID = {
  0x9e8d8d40,
  0xe4d1,
  0x3547,
  0xad, 0x9e,
  {0x86, 0x52, 0x3f, 0x5e, 0x2f, 0x4a}
};
  
  const size_t volume = disk_size / GPT_SECTOR_SIZE;

  // size is in MiB.
  boot_size *= 1024 * 1024;
  if (boot_size + GPT_SECTOR_SIZE * 64 > disk_size) {
    fprintf(stderr, "Boot partition too large\n");
    goto fail;
  }
  
  config.buf = buf;
  config.volume = volume;
  config.numPart = 2;
  config.partitions = &pc[0];
  do {
    struct PartitionConfig *ptr = config.partitions;
    ptr->name = "Boot Partition";
    ptr->volume = boot_size / GPT_SECTOR_SIZE;
    ptr->partType = EFI_SYSTEM_PARTITION;
    ptr->partId = myDiskID; 

    // next partition.
    ptr++;
    ptr->name = "Filesystem";
    ptr->volume = volume - 5 - pc[0].volume;
    ptr->partType = EFI_SYSTEM_PARTITION;
    ptr->partId = myDiskID; 
  } while (0);
  config.diskId = myDiskID;

  if (config.numPart != GPTGenerate(&config)) {
    fprintf(stderr, "an error ocurred creating GPT\n");
    goto fail;
  }
    
  // ok.
  return;
fail:
    munmap(buf, disk_size);
    _exit(1);
}

static void copyin(void *fat, const char *src, const char *name) {
  int fd = open(src, O_RDONLY);
  const uint16_t sysFlag = FAT_ATTR_READ_ONLY | FAT_ATTR_SYSTEM;
  if (fd < 0) {
    die("open");
  }

  // open a file descriptor, and copy into the fs.
  int ret = FAT32Copyin(fat, fd, name, sysFlag);
  if (ret == 0) {
    fprintf(stderr, "disk full");
    die(NULL);
  } 
  printf("%s -> %d\n", src, ret);
  close(fd);
  fflush(stdin);
}

static void usage(FILE *stream) {
  static const char *usage_string = "Usage:\n"
    "\t-k path to linux bzImage\n"
    "\t-d destination (a image file or a block device)\n"
    "\t-s size of first partition in MiB (Default 64)\n"
    "\t-busybox path to static-linked busybox executable\n";

  fputs(usage_string, stream);
}

int main(int argc, char **argv) {
  const char *path_bzimage = NULL;
  const char *path_busybox = NULL;
  const char *dst = NULL;
  size_t boot_size = 64;
  bool print_help = false;;

  // parse arguments
  for (int i = 1; i < argc; ) {
    const char *cur = argv[i];
    if (strcmp(cur, "-h") == 0 || strcmp(cur, "--help") == 0) {
      print_help = true;
      break;
    }
    if (strcmp(cur, "-k") == 0) {
       path_bzimage = argv[i + 1];
       i += 2;
    } else if (strcmp(cur, "-d") == 0) {
       dst = argv[i + 1];
       i += 2;
    } else if (strcmp(cur, "-s") == 0) {
       boot_size = atoi(argv[i + 1]);
       i += 2;
    } else if (strcmp(cur, "-busybox") == 0) {
       path_busybox = argv[i + 1];
       i += 2;
    }
  }
  //// handle incorrect usage
  if (print_help) {
    usage(stdout);
    return 0;
  }
  if (path_bzimage == NULL) {
    fprintf(stderr, "Error: no bzimage provided.\n");
    usage(stderr);
    return 1;
  } if (dst == NULL) {
    fprintf(stderr, "Error: no destination provided.\n");
    usage(stderr);
    return 1;
  } if (path_busybox == NULL) {
    fprintf(stderr, "Error: no busybox provided.\n");
    usage(stderr);
    return 1;
  }

  int dst_fd = open(dst, O_RDWR);
  if (dst_fd < 0) {
    die("open dest");
  }
  //// round down disk size fo a page boundary.
  disk_size = fdsize(dst_fd, dst) / 4096 * 4096;
  buf = mmap(NULL, disk_size, PROT_READ | PROT_WRITE,
    MAP_SHARED, dst_fd, 0);
  if (buf == ((void *) -1)) {
    die("mmap");
  }
  
  // create a gpt partition table.
  printf("boot partition -> %ld MiB\n", boot_size);
  mkpart(boot_size);

  // Create a fat32 filesystem on the partition
  static struct FATConfig fconfig;
  fconfig.nSector = config.partitions[0].volume;
  fconfig.bytesPerSector = GPT_SECTOR_SIZE;
  fconfig.clusterSize = 8;
  fconfig.writeRandomData = false;
  fconfig.isFixed = true;
  void *sectorForFAT = buf + (GPT_SECTOR_SIZE * (pc[0].startLBA));
  createFAT32(sectorForFAT, &fconfig);
  
  // Copy system files into the FAT32 partition
  //// copy secondary bootloader(an ELF loader, but do not parse
  //// file system)
  copyin(sectorForFAT, "linuz.bin", "loader");
  //// copy the kernel bzImage
  copyin(sectorForFAT, path_bzimage, "kernel");
  //// copy busybox, a versatile utility
  copyin(sectorForFAT, path_busybox, "busybox");
  //// copy mbr boot sector
  copyin(sectorForFAT, "init.elf", "init");
  //// copy boot code into MBR. 
  int mbr_fd = open("mbr.img", O_RDONLY);
  if (read(mbr_fd, buf, 440) < 0) {
    die("read mbr.img");
  }
  close(mbr_fd);
  
  munmap(buf, disk_size);
  close(dst_fd);
  return 0;
}

