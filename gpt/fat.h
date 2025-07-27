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


/** Creates a fat32 file system on buf. 
 * @return 0 on success.
 */
extern int createFAT32(uint8_t *buf, const struct FATConfig *config);

#endif // FAT_H
