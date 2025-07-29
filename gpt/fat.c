// Create a fresh fat filesystem

#include "endian.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int FAT32Errno = 0;

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
  // 0xF8 is the standard value for "fixed" (non-removable) media. 
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
  // create a copy at sector #6.
  struct BPB *bpbCopy = (struct BPB *)(buf + (sectorSize * 6));
  memcpy(bpbCopy, bpb, sizeof(*bpbCopy));
  /** End initialize Bios parameter block */

  /** Initialize FSInfo block(at #1, #7). */
  struct FSInfo *finfo = (struct FSInfo *)(buf + (sectorSize * 1));
  _generic_store_le(finfo->leadSignature, 0x41615252);
  _generic_store_le(finfo->structSignature, 0x61417272);
  _generic_store_le(finfo->freeCount, numFreeFATEntry);
  _generic_store_le(finfo->nextFree, 0x3);
  _generic_store_le(finfo->trailSignature, 0xAA550000);
  struct FSInfo *finfoCopy = (struct FSInfo *)(buf + (sectorSize * 7));
  memcpy(finfoCopy, finfo, sizeof(*finfo));
  /** End Initialize FSInfo block. */

  /** Initialize File allocation table. */
  const uint32_t fat1Value = FAT32_CHUNK_SHUT_BITMASK | FAT32_HDR_ERROR_BITMASK;
  memset(buf + (sectorSize * numResevedSector),
  0, sectorSize * numFATSector);
  fat32_entry_t *firstTable = (fat32_entry_t *)
   (buf + (sectorSize * numResevedSector));
  _generic_store_le(firstTable[0], (0xFFFF00) | media);
  _generic_store_le(firstTable[1], fat1Value);
  _generic_store_le(firstTable[2], 0xFFFFFFFF);
  fat32_entry_t *secondTable = (fat32_entry_t *)
    (buf + (sectorSize * (numResevedSector + fatSizeInSector)));
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
  uint8_t createTimeTenth;


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

	int srcFd = open("/usr/bin/echo", O_RDONLY);
	uint32_t sector = (FAT32Copyin(buf, srcFd, "echo", 
					FAT_ATTR_READ_ONLY | FAT_ATTR_SYSTEM));
	close(srcFd);
  assert(sector != 0 && "something went wrong");
  // this will print the sector where echo 
  // is located in the image.
  printf("%u\n", sector);

  /**< check return value of write */
  if (write(fd, buf, volume) != volume) {
    perror("write");
    free(buf);
    return 1;
  };
  free(buf);
  close(fd);

  return 0;
}

uint16_t FAT32Date(uint16_t year, uint16_t month, uint16_t day) {
  /**
   * Bit positions 0 through 4 represent the day of the month (valid range: 1..31 inclusive) 
   * Bit positions 5 through 8 represent the month of the year (1 = January, 12 = December, valid 
   * range: 1..12 inclusive) 
   * Bit positions 9 through 15 are the count of years from 1980 (valid range: 0..127 inclusive 
   * allowing representation of years 1980 through 2107)
   */

  uint16_t ret = 0;

  ret |= (day & 0x1F);
  ret |= ((month & 0xF) << 5u);
  ret |= ((year & 0x7F) << 9u);

  return ret;
}

uint16_t FAT32Time(uint16_t hour, uint16_t minute, uint16_t second) {
  /**
   * Bit positions 0 through 4 contain elapsed seconds – as a count of 2-second increments (valid 
   * range: 0..29 inclusive allowing representation of 0 through 58 seconds) 
   *
   * Bit positions 5 through 10 represent number of minutes (valid range: 0..59 inclusive) 
   * Bit positions 11 through 15 represent hours (valid range: 0..23 inclusive)
   */

  uint16_t ret = 0;
  second = second >> 1;

  ret |= (second & 0x1F); /* 5 bit */
  ret |= ((minute & 0x3F) << 5u); /* 6 bit */
  ret |= ((hour & 0x1F) << 11u); /* 5 bit */

  return ret;
}

/**< Information requried to create file or directory. */
struct FAT32Super {
  uint8_t *buf; /**< buffer containint disk image */

  struct BPB *bpb;        /**< Sector #0 */
  struct FSInfo *fsInfo;  /**< Sector #1 */

  uint32_t bytesPerSector;  /**< size of sector */
  uint32_t bytesPerCluster; /**< size of cluster */
  uint32_t bytesPerFAT;     /**< size of FAT */

  uint32_t numSector;   /**< Number of all sectors */
  uint32_t numReserved; /**< number of sectors in reserved region,
                            also FAT offset */
  uint32_t numFAT;      /**< number of sectors in FAT region */
  uint32_t numData;     /**< number of sectors in data region */
  uint32_t maxCluster;  /**< maximum cluster number */

	uint32_t rootCluster; /**< position of root */

  uint32_t numEntryTable;     /**< number of fat tables. */
  fat32_entry_t **tables;     /**<  all fat tables */

	struct FATDirEntry *rootDir; /**< entries in root */
	uint32_t nextRootEntry;      /**< next root entry to alloc */
	uint32_t numRootEntry;       /**< number of root entries */
};

static void FAT32SuperInit(struct FAT32Super *sb, uint8_t *buf);
static void *FAT32SuperGetSector(const struct FAT32Super *sb,
              uint32_t sector);
static uint8_t *FAT32SuperCluster(const struct FAT32Super *sb, 
                                  uint32_t cluster);
static void FAT32SuperFree(struct FAT32Super *sb);
static fat32_entry_t FAT32SuperReadEntry(const struct FAT32Super *sb, uint32_t cluster);
static void FAT32SuperWriteEntry(const struct FAT32Super *sb,
  uint32_t cluster, fat32_entry_t value);

static void FAT32SuperInit(struct FAT32Super *sb, uint8_t *buf) {
  sb->buf = buf;

  struct BPB *bpb = (struct BPB *) buf;
  sb->bpb = bpb;
  // sb->fsInfo cannot be initialized here.

  _generic_load_le(sb->bytesPerSector, bpb->bytesPerSector);
  uint32_t nsecPerCluster; /**< bpb->numSectorPerCluster */
  _generic_load_le(nsecPerCluster, bpb->numSectorPerCluster);
  sb->bytesPerCluster = nsecPerCluster * sb->bytesPerSector;
  uint32_t fatSize; /**< bpb->fatSize32 */
  _generic_load_le(fatSize, bpb->fatSize32);
  sb->bytesPerFAT = fatSize * sb->bytesPerSector;

  uint32_t numSector; /**< bpb->numSector32 */
  uint32_t numReservedSectors; /**< bpb->numReservedSector */
  uint32_t numAllocTable; /**< bpb->numAllocTable  */
  _generic_load_le(numSector, bpb->numSectors32);
  _generic_load_le(numReservedSectors, bpb->numReservedSectors);
  _generic_load_le(numAllocTable, bpb->numAllocTable);
  sb->numSector = numSector;
  sb->numReserved = numReservedSectors;
  sb->numFAT = fatSize * numAllocTable;
  sb->numData = (sb->numSector - sb->numReserved - sb->numFAT);
  sb->maxCluster = sb->numData / nsecPerCluster;

  sb->fsInfo = FAT32SuperGetSector(sb, 1);
	_generic_load_le(sb->rootCluster, bpb->rootCluster);

  sb->numEntryTable = numAllocTable;
  sb->tables = malloc(sizeof(fat32_entry_t *) * sb->numEntryTable);
  sb->tables[0] = (fat32_entry_t *)(buf + sb->bytesPerSector * sb->numReserved);

  for (uint32_t i = 1; i < numAllocTable; i++) {
    sb->tables[i] = sb->tables[i - 1] + (sb->bytesPerFAT / sizeof(fat32_entry_t));
  }

	uint32_t rootDirCluster = 1;
	fat32_entry_t rootDirPtr = sb->rootCluster;
	fat32_entry_t nextPtr;
	_generic_load_le(nextPtr, sb->tables[0][rootDirPtr]);
	while (nextPtr != 0xFFFFFFFFu) {
		rootDirPtr = nextPtr;
		_generic_load_le(nextPtr, sb->tables[0][rootDirPtr]);
		rootDirCluster += 1;
	}
	sb->rootDir = (struct FATDirEntry *)FAT32SuperGetSector(sb, sb->numReserved + sb->numFAT);
	sb->numRootEntry = rootDirCluster * sb->bytesPerCluster / sizeof(struct FATDirEntry);
	sb->nextRootEntry = sb->numRootEntry;

	for (uint32_t i = 0; i < sb->numRootEntry; i++) {
		struct FATDirEntry *cur = &(sb->rootDir[i]);
		if (cur->name[0] == 0) {
			sb->nextRootEntry = i;
			break;
		}
	}
}

/**
 * @return pointer to a sector.
 */
static void *FAT32SuperGetSector(const struct FAT32Super *sb,
              uint32_t sector) {
  
  if (sector >= sb->numSector) {
    return NULL;
  }
  return sb->buf + (sector * sb->bytesPerSector);
}

/**
 * @return pointer to cluster `cluster`
 */
static uint8_t *FAT32SuperCluster(const struct FAT32Super *sb, 
                                  uint32_t cluster) {
  if (cluster < 2 || cluster >= sb->maxCluster) {
    return NULL;
  }

  return sb->buf + (
    sb->bytesPerSector * (sb->numFAT + sb->numReserved)  /**< data region offset */
  + sb->bytesPerCluster * (cluster - 2) /**< cluster offset */
  );
}

/** Read an fat entry from first table. */
static fat32_entry_t FAT32SuperReadEntry(const struct FAT32Super *sb, 
  uint32_t cluster) {
  assert(cluster < sb->maxCluster && "read out of bound");
  fat32_entry_t ret;
  _generic_load_le(ret, sb->tables[0][cluster]);
  return ret;
}

/**< Writes new value to an FAT entry on all FATs. */
static void FAT32SuperWriteEntry(const struct FAT32Super *sb,
  uint32_t cluster, fat32_entry_t value) {
  assert(cluster < sb->maxCluster && "write out of bound");
  assert(cluster >= 1 && "entry 0 cannot be modified");

  for (uint32_t i = 0; i < sb->numEntryTable; i++) {
    _generic_store_le(sb->tables[i][cluster], value);
  }
}

/**< Update backup FAT, FSInfo and BPB */
static void FAT32SuperFree(struct FAT32Super *sb) {
  // synchronize BPB(#0 -> #6) 
  memcpy(FAT32SuperGetSector(sb, 6), sb->buf, sb->bytesPerSector);

  // synchronize FSInfo(#1->#7)
  memcpy(FAT32SuperGetSector(sb, 7),
         FAT32SuperGetSector(sb, 1), sb->bytesPerSector);

  // free memory buffer.
  free(sb->tables);
}

int FAT32Mkdir(uint8_t *buf, const char *name, uint32_t flag) {
  // not implemented
  return 1;
}

uint32_t FAT32Copyin(uint8_t *buf, int fd, const char *name, 
			        	uint32_t attr) {

	uint32_t ret = 0;
	
	// infer the size of file to be put.
	// and possibly reject huge files.
	struct stat st;
	if (fstat(fd, &st) != 0) {
		// bad fd.
		FAT32Errno = errno;
		return 0;
	}
	uint32_t fobjSize = st.st_size;

	// parse the super block.
	struct FAT32Super sb;
	FAT32SuperInit(&sb, buf);
	const uint32_t sectorPerCluster = 
					(sb.bytesPerCluster / sb.bytesPerSector);
	
	// validate the size of the file.
	struct FSInfo *fiPtr = sb.fsInfo;
	uint32_t nextFree, freeCount;
	_generic_load_le(nextFree, fiPtr->nextFree);
	_generic_load_le(freeCount, fiPtr->freeCount);
	assert(sb.maxCluster >= nextFree && 
									"incorrect fs");
	const uint32_t maxFileSize = 
					sb.bytesPerCluster * 
					(sb.maxCluster - nextFree);
	if (maxFileSize < fobjSize) {
		// die.
		FAT32Errno = EDQUOT;
		goto err;
	}

	// use the fsInfo struct to allocate blocks.
	const uint32_t allocCluster = CDIV(fobjSize, sb.bytesPerCluster);

	// If root directory is full. do not try 
	// to expand it, but fail now.
	if (sb.nextRootEntry == sb.numRootEntry) {
		FAT32Errno = EDQUOT;
		goto err;
	}

	// find the pointer to the first cluster 
	// allocated, and copy the data.
	assert(nextFree > 2u);
	ret = (nextFree - 2u) * sectorPerCluster;
	ret += sb.numReserved + sb.numFAT;
	uint8_t *addr = FAT32SuperGetSector(&sb, ret);
	memset(addr, 0, allocCluster * sb.bytesPerCluster);
	if (lseek(fd, SEEK_SET, 0) != 0) {
		FAT32Errno = errno;
		goto err;
	}
	if (read(fd, addr, fobjSize) != fobjSize) {
		FAT32Errno = errno;
		goto err;
	}

	// mark allocated clusters in
	// FAT table.

	// finalize this transaction
	// by creating a directory entry.
	// assume that the root directory
	const uint16_t date = FAT32Date(1995, 02, 21);
	const uint16_t time = FAT32Time(8, 30, 25);
	struct FATDirEntry *entry = &(sb.rootDir[sb.nextRootEntry]);
	assert(entry->name[0] == 0);
	memset(entry, 0, sizeof(*entry));
	//// copy the prefix of filename
	char *pt = entry->name;
	for (; *name && *name != '.'; name++) {
		*pt = *name;
		pt++;
	}
	//// copy suffix of filename
	if (*name) {
		pt = ((char *)entry->name) + 8;
		for (; *name; name++) {
			*pt = *name; pt++;
		}
	}
	//// set other numeric attributes.
	_generic_store_le(entry->attr, attr);
	_generic_store_le(entry->ntres, 0);
	_generic_store_le(entry->createTimeTenth, 42);
	_generic_store_le(entry->createTime, time);
	_generic_store_le(entry->createDate, date);
	_generic_store_le(entry->lastAccessDate, date);
	_generic_store_le(entry->firstClusterHigh, nextFree >> 16);
	_generic_store_le(entry->firstClusterLow, nextFree & 0xFFFFu);
	_generic_store_le(entry->fileSize, fobjSize);
	sb.nextRootEntry ++;

  // update FAT Table.
  for (uint32_t i = 1; i < allocCluster; i++) {
    FAT32SuperWriteEntry(&sb, nextFree, 1 + nextFree);
    nextFree++;
  }
  FAT32SuperWriteEntry(&sb, nextFree, 0xFFFFFFFFu);

	// update fs info struct
	assert(freeCount >= allocCluster);
	freeCount -= allocCluster;
	_generic_store_le(fiPtr->freeCount, freeCount);
	_generic_store_le(fiPtr->nextFree, nextFree);

	// clean, return success.
	FAT32SuperFree(&sb);
	return ret;

err:
	/** an error occurred */
	FAT32SuperFree(&sb);
	return 0;
}

