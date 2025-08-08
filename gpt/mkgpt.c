#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fat.h"
#include "gpt.h"

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

int main(int argc, char **argv) {
  static struct PartitionConfig pc[2];  
  static struct GPTConfig config;
  // suppose we have a disk of this size(in bytes)
  // we mmap() a buffer representing the disk,
  // (zero initialized buffer)
  const size_t disk_size = 1024 * 1024 * 256;
  const size_t volume = disk_size / GPT_SECTOR_SIZE;
  const char *perror_message;

  printf("#ifndef __CONFIG_H\n#define __CONFIG_H\n\n");

  // allocate resources.
  int fd = open("a.img", O_CREAT | O_TRUNC | O_RDWR, 0777);
  if (fd < 0) {
    perror_message = "open";
    goto fail;
  }
  void *buf = mmap(NULL, disk_size, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (buf == (void *) -1) {
    perror_message = "mmap";
    goto fail;
  }

  // initialize structs.
  config.buf = buf;
  config.volume = volume;
  config.numPart = 2;
  config.partitions = &pc[0];
  do {
    struct PartitionConfig *ptr = config.partitions;
    ptr->name = "Boot Partition";
    ptr->volume = 262140; // 1024 * 1024 * 64 / GPT_SECTOR_SIZE;
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

  // create partiton table.
  if (config.numPart != GPTGenerate(&config)) {
    write(2, "Error: disk full\n", 18);
    goto local_fail;
  }

  // write the start sector of each partition
  for (size_t i = 0; i < config.numPart; i++) {
    printf("#define PARTITION_%ld_START %ld\n", i, config.partitions[i].startLBA);
  }

  /** Create a fat32 filesystem on the partition */
  static struct FATConfig fconfig;
  fconfig.nSector = config.partitions[0].volume;
  fconfig.bytesPerSector = GPT_SECTOR_SIZE;
  fconfig.clusterSize = 8;
  fconfig.writeRandomData = false;
  fconfig.isFixed = true;
  void *sectorForFAT = config.buf + (GPT_SECTOR_SIZE * (pc[0].startLBA));
  createFAT32(sectorForFAT, &fconfig);
  
  // Copy system files into the FAT32 partition
  //// copy secondary bootloader(an ELF loader, but do not parse
  //// file system)
  const uint16_t sysFlag = FAT_ATTR_READ_ONLY | FAT_ATTR_SYSTEM;
  int fatFd = open("../bootloader/linuz.bin", O_RDONLY);
  printf("#define BOOTLOADER_OFFSET %d\n", FAT32Copyin(sectorForFAT, fatFd, "linuz", sysFlag));
  close(fatFd);
  //// copy the kernel ELF
  fatFd = open("../bootloader/kernel", O_RDONLY);
  printf("#define KERNEL_OFFSET %d\n", FAT32Copyin(sectorForFAT, fatFd, "kernel", sysFlag));
  close(fatFd);
  /// copy busybox, a versatile utility
  fatFd = open("/usr/bin/busybox", O_RDONLY);
  printf("#define BUSYBOX_OFFSET %d\n", FAT32Copyin(sectorForFAT, fatFd, "busybox", FAT_ATTR_READ_ONLY));
  close(fatFd);
  fatFd = open("init", O_RDONLY);
  printf("#define INIT_OFFSET %d\n", FAT32Copyin(sectorForFAT, fatFd, "init", FAT_ATTR_READ_ONLY));
  close(fatFd);
  //// copy mbr boot sector
  fatFd = open("mbr.img", O_RDONLY);
  read(fatFd, buf, 440);
  close(fatFd);

  // write to disk.
  if (write(fd, buf, disk_size) != disk_size) {
    perror_message = "write";
    goto fail;
  }
  printf("\n#define BOOT_PARTITION_START PARTITION_0_START\n");
  printf("\n#endif // __CONFIG_H\n");

  close(fd);
  munmap(buf, disk_size);
  return 0;
  // exited with 0.

fail:
  perror(perror_message);
local_fail:
  if (config.buf)
    munmap(buf, disk_size);
  close(fd);
  _exit(1);
}
