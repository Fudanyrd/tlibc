// Create a fresh fat filesystem

#include "endian.h"

#include <stdbool.h>

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

  // number of allocation tables.
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
  uint32_t numSector32;

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
    uint8_t volLabel[11];

    // set to the string "FAT32   ",
    // hex: 46 41 54 33 32 20 20 20
    uint8_t fileSysType[8];

    // set to 0
    uint8_t reserved3[420];

    // set to 0x55(at offset 510) 
    uint8_t signatureByte1;

    // set to 0x55(at offset 510) 
    uint8_t signatureByte2;

  } __attribute__((packed));

} __attribute__((packed));

int main(int argc, char **argv) {

  printf("%ld\n", sizeof(struct BPB));
  assert(sizeof(struct BPB) == 512);
  return 0;
}
