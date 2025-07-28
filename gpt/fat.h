#ifndef FAT_H
#define FAT_H

#include <stdbool.h>
#include <stdint.h>

struct FATConfig {
  // number of sectors.
  uint32_t nSector; 

  uint32_t bytesPerSector;


  // size of a cluster
  uint32_t clusterSize; 

  // if true, write random data to data region.
  bool writeRandomData;

  // if true, the media is fixed, else removable.
  bool isFixed;
};

#define FAT_ATTR_READ_ONLY 0x01 
#define FAT_ATTR_HIDDEN 0x02 
#define FAT_ATTR_SYSTEM 0x04 
#define FAT_ATTR_VOLUME_ID 0x08 
#define FAT_ATTR_DIRECTORY 0x10 
#define FAT_ATTR_ARCHIVE 0x20

/** Creates a fat32 file system on buf. 
 * @return 0 on success.
 */
extern int createFAT32(uint8_t *buf, const struct FATConfig *config);

#endif // FAT_H
