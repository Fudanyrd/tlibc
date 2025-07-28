// Create a fresh fat filesystem

#include "endian.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

static const char *FAT_IMPL = "mkfs.fat";

// NOTE:
// All of the FAT file systems were originally developed for the IBM PC machine architecture.
// Hence, on disk data structures for the FAT format are all “little endian.”

// clang-format off
/**
 * Fat fs layout:
 * +--------------------+-----------------------+------+
 * | BPB | ...    | BPB | AllocTable | FAT Copy | Data |
 * +--------------------+-----------------------+------+
 * ^reserved region     ^FAT region             ^ file-dir region
 */
// clang-format on

// BIOS Parameter Block, simply the first block.
struct BPB {

  // Jump instruction to boot code.
  // allowed value: {0xeb, 0x??, 0x90} or {0xe9, 0x??, 0x??}
  uint8_t jmpBoot[3];

  // OEM Name Identifier.
  // can be set by a FAT implementation
  uint8_t OEFName[8];

  // Count of bytes per sector.
  // must be 512, 1024, 2048, 4096
  uint16_t bytesPerSector;

  // number of sectors per allocation unit.
  // must be 1, 2, 4, 8, ... 128
  uint8_t numSectorPerCluster;

  // number of reserved sectors
  // can be any nonzero value
  uint16_t numReservedSectors;

  // number of file allocation tables(FATs).
  // a value of 2 is recommended
  uint8_t numAllocTable;

  // For FAT12 and FAT16 volumes, this field contains 
  // the count of 32-byte directory entries in the root 
  // directory. For FAT32 volumes, this field must be set 
  // to 0.
  uint16_t rootEntryCount;

  // total count of sectors on volume.
  //
  // This field can be 0; if it is 0, then numSector32 
  // must be non-zero. For FAT32 volumes, this field 
  // must be 0.
  //
  // For FAT12 and FAT16 volumes, this field contains 
  // the sector count, and BPB_TotSec32 is 0 if the 
  // total sector count “fits” (is less than 0x10000)
  uint16_t numSectors16;

  // The legal values for this field are 0xF0, 0xF8, 0xF9, 
  // 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, and 0xFF. 
  //
  // 0xF8 is the standard value for “fixed” (non-removable) media. 
  // For removable media, 0xF0 is frequently used.
  uint8_t media;

  // This field is the FAT12/FAT16 16-bit count of 
  // sectors occupied by one FAT. On FAT32 volumes 
  // this field must be 0, and BPB_FATSz32 contains 
  // the FAT size count.
  uint16_t fatSize16;

  // Sectors per track for interrupt 0x13. 
  // This field is only relevant for media that have a 
  // geometry (volume is broken down into tracks by 
  // multiple heads and cylinders) and are visible on 
  // interrupt 0x13.
  uint16_t sectorsPerTrack;

  // Number of heads for interrupt 0x13. This field is 
  // relevant as discussed earlier for BPB_SecPerTrk. 
  // This field contains the one based “count of heads”.
  // For example, on a 1.44 MB 3.5-inch floppy drive 
  // this value is 2.
  uint16_t numHeads;

  // Count of hidden sectors preceding the partition that 
  // contains this FAT volume. This field is generally 
  // only relevant for media visible on interrupt 0x13. 
  // This field must always be zero on media that are 
  // not partitioned. 
  //
  // NOTE: Attempting to utilize this field to align the
  // start of data area is incorrect.
  uint32_t hiddenSec;

  // This field is the new 32-bit total count of sectors on 
  // the volume. This count includes the count of all 
  // sectors in all four regions of the volume. 
  //
  // This field can be 0; if it is 0, then BPB_TotSec16
  // must be non-zero. For FAT12/FAT16 volumes, this 
  // field contains the sector count if BPB_TotSec16 is 
  // 0 (count is greater than or equal to 0x10000). 
  //
  //For FAT32 volumes, this field must be non-zero.
  uint32_t numSectors32;

  // held by fat32 format
  struct {
    // This field is the FAT32 32-bit count of sectors occupied 
    // by one FAT. 
    //
    // Note that BPB_FATSz16 must be 0 for media formatted
    // FAT32.
    uint32_t fatSize32;

    // Bits 0-3 -- Zero-based number of active FAT. Only 
    // valid if mirroring is disabled. 
    //
    // Bits 4-6 -- Reserved. 
    //
    // Bit 7 -- 0 means the FAT is mirrored at runtime into 
    // all FATs; 1 means only one FAT is active; it is the one 
    // referenced in bits 0-3. 
    //
    // Bits 8-15 -- Reserved.
    uint16_t extFlags;

    // High byte is major revision number. Low byte is minor 
    // revision number. This is the version number of the 
    // FAT32 volume. 
    //
    // Must be set to 0x0
    uint16_t revision;

    // This is set to the cluster number of the first cluster of 
    // the root directory, 
    // This value should be 2 or the first usable (not bad) 
    // cluster available thereafte
    uint32_t rootCluster;

    // Sector number of FSINFO structure in the reserved area 
    // of the FAT32 volume. Usually 1.
    uint16_t fsInfo;

    // Set to 0 or 6. 
    //
    // If non-zero, indicates the sector number in the 
    // reserved area of the volume of a copy of the boot 
    // record.
    //
    // To avoid loss of data, sector 6 must contain a copy
    // of BPB, this field contains value 6 for both copy.
    uint16_t bootRecordSector;

    // Reserved. Must be set to 0x0
    uint8_t reserved[12];
    
    // Interrupt 0x13 drive number. Set value to 0x80 or 
    // 0x00.
    uint8_t driverNum;

    // set to 0x0;
    uint8_t reserved1;

    // Extended boot signature. Set value to 0x29 if either of 
    // the following two fields are non-zero. 
    // This is a signature byte that indicates that the 
    // following three fields in the boot sector are present.
    uint8_t bootSignature;

    // volume serial number
    uint32_t volSerialNumber;

    // volume label
    // This field matches the 11-byte 
    // volume label recorded in the root directory
    // 
    // The setting for this field when there is no 
    // volume label is the string “NO NAME ”
    uint8_t volLabel[11];

    // set to the string "FAT32   ",
    // hex: 46 41 54 33 32 20 20 20
    uint8_t fileSysType[8];

    // set to 0
    uint8_t reserved3[420];

    // set to 0x55(at offset 510) 
    uint8_t signatureByte1;

    // set to 0xaa(at offset 511) 
    uint8_t signatureByte2;

  } __attribute__((packed));

} __attribute__((packed));

/**
 * In FAT32, each fat entry is 32bit. `MAX`
 * is the maximum possible cluster number.
 * The first data cluster in the volume is cluster #2.
 * 
 * FAT[0] contains the BPB_Media byte value in its low 8 bits, and all 
 * other bits are set to 1. For example, if BPB_Media is 0xF8, then it 
 * should be 0xFFFFF8.
 * 
 * FAT[1] is set to the EOC Value.
 * 
 * 0x0, free
 * 
 * [0x2, MAX], cluster is allocated and
 * Value of the entry is the cluster number of 
 * the next cluster following this 
 * corresponding cluster.
 * 
 * 0xFFFFFF7 bad cluster
 * 
 * 0xFFFFFFFF allocated and is final cluster of the file.
 */
typedef uint32_t fat32_entry_t;

/**
 * Used in FAT[1].
 * 
 * If bit is 1, the volume is “clean”. The volume can be mounted for 
 * access. If bit is 0, the volume is “dirty” indicating that a FAT file 
 * system driver was unable to dismount the volume properly (during a 
 * prior mount operation). The volume contents should be scanned for 
 * any damage to file system metadata
 */
#define FAT32_CHUNK_SHUT_BITMASK ((uint32_t) 0x08000000)

/**
 * Used in FAT[1] of fat32.
 * 
 * If this bit is 1, no disk read/write errors were encountered. 
 * If this bit is 0, the file system driver implementation encountered a 
 * disk I/O error on the volume the last time it was mounted, which is an 
 * indicator that some sectors may have gone bad. The volume 
 * contents should be scanned with a disk repair utility that does 
 * surface analysis on it looking for new bad sectors.
 */
#define FAT32_HDR_ERROR_BITMASK ((uint32_t) 0x04000000)

/**
 * @return the number of sectors in the data region.
 */
static uint32_t numSectorDataRegion(const struct BPB *bpb) {
  uint32_t BPB_BytsPerSec = bpb->bytesPerSector;
  uint32_t BPB_RootEntCnt = bpb->rootEntryCount;
  uint32_t FATSize;
  uint32_t totalSec;

  if (bpb->fatSize16 != 0) {
    FATSize = bpb->fatSize16;
  } else {
    FATSize = bpb->fatSize32;
  }

  if (bpb->numSectors16) {
    totalSec = bpb->numSectors16;
  } else {
    totalSec = bpb->numSectors32;
  }

  uint32_t dataSec = totalSec - (
  bpb->numReservedSectors /* size of reserved region */
   + (FATSize * bpb->numAllocTable) /* size of FAT region */
   + BPB_RootEntCnt /** is zero for fat32 */);
  assert(dataSec < totalSec && "u32 overflow");

  return dataSec;
}

/**
 * @param n a valid cluster number
 * @return sector number for that sector(high 32 bits) 
 *   and the offset in the sector(lower 32bits).
 */
static uint64_t clusterToFATEntry(const struct BPB *bpb, uint32_t n) {
  uint64_t ret = 0;
  uint32_t FATSize;
  uint32_t fatOffset;

  if (bpb->fatSize16 != 0) {
    FATSize = bpb->fatSize16;
    fatOffset = n * 2;
  } else {
    FATSize = bpb->fatSize32;
    fatOffset = n * 4;
  }
  assert(FATSize != 0 && "fatsize cannot be 0");

  uint32_t secNum = bpb->numReservedSectors + 
    (fatOffset / bpb->bytesPerSector);
  uint32_t fatEntOffset = fatOffset % bpb->bytesPerSector;
  
  ret |= (((uint64_t) secNum) << 32);
  ret |= ((uint64_t) fatEntOffset);

  /** Takeaway here:
   * starting from the first sector of alloc table,
   * the entries are organized in an array format.
   */
  return ret;
}


// The FSInfo structure is only present on volumes formatted FAT32. 
// The structure must be persisted on the media during volume 
// initialization (format). The structure must be located at 
// sector #1 – immediately following the sector containing the BPB. 
// A copy of the structure is maintained at sector #7.
struct FSInfo {

  // Value = 0x41615252
  uint32_t leadSignature;

  // Reserved, set to 0
  uint8_t reserved[480];

  // Value is 0x61417272.
  uint32_t structSignature;

  // Contains the last known free cluster count on the  volume. 
  // The value 0xFFFFFFFF indicates the free count is unknown
  //
  // The contents of this field must be validated at 
  // volume mount (and subsequently maintained in 
  // memory by the file system driver implementation
  uint32_t freeCount;

  // Contains the cluster number of the first available 
  // (free) cluster on the volume. 
  // The value 0xFFFFFFFF indicates that there exists 
  // no information about the first available (free) 
  // cluster. 
  //
  // The contents of this field must be validated at 
  // volume mount.
  uint32_t nextFree;

  // must be 0
  uint8_t reserved1[12];

  // Value = 0xAA550000
  uint32_t trailSignature;

} __attribute__((packed));

#include "fat.h"

/**
 * ceiling divisoin
 */
#define CDIV(a, b) ( ((a) + (b) - 1) / (b) )

/** Creates a fat32 file system on buf. 
 * @return 0 on success.
 */
int createFAT32(uint8_t *buf, const struct FATConfig *config) {
  
  /** Begin validating config. */
  if (config->nSector < 16) {
    // too small.
    return 1;
  } 

  bool bytesIsValid = false;
  for (int i = 512; i < 8192; i *= 2) {
    if (config->bytesPerSector == i) {
      bytesIsValid = true;
      break;
    }
  }
  if (!bytesIsValid) {
    // invalid option to config->bytesPerSector.
    // must be a power of 2 and in [512, 4096]
    return 1;
  }

  bool clusterSizeIsValid = false;
  for (int i = 1; i < 256; i *= 2) {
    if (config->clusterSize == i) {
      clusterSizeIsValid = true;
      break;
    }
  }
  if (!clusterSizeIsValid) {
    // invalid option to config->bytesPerSector.
    // must be a power of 2 and in [1, 128]
    return 1;
  }
  /** End validating config. */

  /** Begin initialize Bios parameter block */
  struct BPB *bpb = (struct BPB *)buf;
  const uint8_t media = config->isFixed ? 0xF8 : 0xF0;
  const uint32_t sectorSize = config->bytesPerSector;
  const uint32_t numEntryPerSector = sectorSize / sizeof(fat32_entry_t);
  const uint32_t numClusterOnDisk = (config->nSector) / (config->clusterSize);
  const uint32_t fatSizeInSector = CDIV(numClusterOnDisk, numEntryPerSector);
  const uint32_t numResevedSector = 8;
  const uint32_t numFATSector = fatSizeInSector * 2 /* a backup is stored. */;
  const uint32_t numFreeFATEntry = (numFATSector * numEntryPerSector) - 3;
  assert(numClusterOnDisk > 2 && "too few clusters");
  memset(bpb, 0, sizeof(*bpb));
  bpb->jmpBoot[0] = 0xeb;
  bpb->jmpBoot[1] = 0x00;
  bpb->jmpBoot[2] = 0x90;
  memcpy(bpb->OEFName, FAT_IMPL, sizeof(bpb->OEFName));
  _generic_store_le(bpb->bytesPerSector, config->bytesPerSector);
  _generic_store_le(bpb->numSectorPerCluster, config->clusterSize);
  _generic_store_le(bpb->numReservedSectors, numResevedSector);
  _generic_store_le(bpb->numAllocTable, 2);
  _generic_store_le(bpb->rootEntryCount, 0);
  _generic_store_le(bpb->numSectors16, 0);
  _generic_store_le(bpb->media, media);
  _generic_store_le(bpb->sectorsPerTrack, 32);
  _generic_store_le(bpb->numHeads, 8);
  _generic_store_le(bpb->hiddenSec, 0);
  _generic_store_le(bpb->numSectors32, config->nSector);
  _generic_store_le(bpb->fatSize32, fatSizeInSector);
  _generic_store_le(bpb->extFlags, 0);
  _generic_store_le(bpb->revision, 0);
  _generic_store_le(bpb->rootCluster, 2);
  _generic_store_le(bpb->fsInfo, 1);
  _generic_store_le(bpb->bootRecordSector, 6);
  _generic_store_le(bpb->driverNum, 0x80);
  _generic_store_le(bpb->bootSignature, 0x29);
  _generic_store_le(bpb->volSerialNumber, 0xe5792f15);
  memcpy(bpb->volLabel, "NO NAME         ", sizeof(bpb->volLabel));
  memcpy(bpb->fileSysType, "FAT32   ", sizeof(bpb->fileSysType));
  _generic_store_le(bpb->signatureByte1, 0x55);
  _generic_store_le(bpb->signatureByte2, 0xaa);
  // create a copy at sector 6.
  struct BPB *bpbCopy = (struct BPB *)(buf + (sectorSize * 6));
  memcpy(bpbCopy, bpb, sizeof(*bpbCopy));
  /** End initialize Bios parameter block */

  /** Initialize FSInfo block(at #1, #7). */
  struct FSInfo *finfo = (struct FSInfo *)(buf + (sectorSize * 1));
  _generic_store_le(finfo->leadSignature, 0x41615252);
  _generic_store_le(finfo->structSignature, 0x61417272);
  _generic_store_le(finfo->freeCount, numFreeFATEntry);
  _generic_store_le(finfo->nextFree, 0x2);
  _generic_store_le(finfo->trailSignature, 0xAA550000);
  struct FSInfo *finfoCopy = (struct FSInfo *)(buf + (sectorSize * 7));
  memcpy(finfoCopy, finfo, sizeof(*finfo));
  /** End Initialize FSInfo block. */

  /** Initialize File allocation table. */
  const uint32_t fat1Value = FAT32_CHUNK_SHUT_BITMASK | FAT32_HDR_ERROR_BITMASK;
  memset(buf + (sectorSize * numResevedSector),
  0, sectorSize * numFATSector);
  fat32_entry_t *firstTable = buf + (sectorSize * numResevedSector);
  _generic_store_le(firstTable[0], (0xFFFF00) | media);
  _generic_store_le(firstTable[1], fat1Value);
  _generic_store_le(firstTable[2], 0xFFFFFFFF);
  fat32_entry_t *secondTable = buf + (sectorSize * 
  (numResevedSector + fatSizeInSector));
  _generic_store_le(secondTable[0], (0xFFFF00) | media);
  _generic_store_le(secondTable[1], fat1Value);
  _generic_store_le(secondTable[2], 0xFFFFFFFF);
  /** End Initialize File allocation table. */

  /** Initialize root directory. */
  const uint32_t dataOffset = sectorSize * (
    numResevedSector + numFATSector);
  memset(buf + dataOffset, 0, sectorSize * config->clusterSize);

  /** created successfully. */
  return 0;
}

/**< Directory entry structure. */
struct FATDirEntry {

  /**
   * 8-byte name, 3-byte suffix.
   */
  uint8_t name[11];

  /**
   * can be set by ATTR_READONLY, ATTR_HIDDEN, etc.
   */
  uint8_t attr;

  /**
   * reserved, set to 0
   */
  uint8_t ntres;

  /**
   * Component of the file creation time. Count of 
   * tenths of a second. Valid range is: 
   * 0 <= DIR_CrtTimeTenth <= 199
   */
  uint8_t creatTimeTenth;


	/**
	 * Creation time. Granularity is 2 seconds.
	 */
	uint16_t createTime;

	/**
	 * FIXME: Add description
	 */
	uint16_t createDate;
	
	/**
	 * Last access date. Last access is defined as a 
	 * read or write operation performed on the 
	 * file/directory described by this entry
	 */
	uint16_t lastAccessDate;

	/**
	 * High word of first data cluster number for 
	 * file/directory described by this entry. 
	 * Only valid for volumes formatted FAT32. Must be 
	 * set to 0 on volumes formatted FAT12/FAT16.
	 */
	uint16_t firstClusterHigh;

	/**
	 * Last modification (write) time. Value must be 
	 * equal to createTime at file creation.
	 */
	uint16_t writeTime;
	
	/**
	 * Last modification (write) date. Value must be
	 * equal to DIR_CrtDate at file creation. 
	 */
	uint16_t writeDate;


	/** 
	 * Low word of first data cluster number for 
   * file/directory described by this entry 
	 */
	uint16_t firstClusterLow;

	/**
	 * Size of the file in bytes.
	 */
	uint32_t fileSize;

} __attribute__((packed));

/** Test code */
__attribute__((weak))
int main(int argc, char **argv) {
  // validate fat32 structs.
  assert(sizeof(uint8_t) == 1);
  assert(sizeof(struct BPB) == 512);
  assert(sizeof(struct FSInfo) == 512);
	assert(sizeof(struct FATDirEntry) == 32);

  /** Create a 128MiB fat32 disk image. */
  const uint32_t FAT32_SECTOR_SIZE = 512;
  int fd = open("myfat.img", O_WRONLY | O_CREAT | O_TRUNC, 0777);
  size_t volume = 128 * (1024 * 1024);
  uint8_t *buf = malloc(volume);
  struct FATConfig config = {
    volume / FAT32_SECTOR_SIZE,
    FAT32_SECTOR_SIZE,
    8,
    false,
    true
  };
  if (createFAT32(buf, &config)) {
    free(buf);
    close(fd);
    assert(0 && "incorrect configuration");
  }
  write(fd, buf, volume);
  free(buf);
  close(fd);

  return 0;
}
