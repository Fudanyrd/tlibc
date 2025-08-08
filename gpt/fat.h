#ifndef FAT_H
#define FAT_H

#include <stdbool.h>
#include <stdint.h>

struct FATConfig {
  // number of sectors.
  uint32_t nSector; 

  uint32_t bytesPerSector;


  // size of a cluster in Sectors
  uint32_t clusterSize; 

  // if true, write random data to data region.
  bool writeRandomData;

  // if true, the media is fixed, else removable.
  bool isFixed;
};


/**
 * The file cannot be modified – all modification 
 * requests must fail with an appropriate error 
 * code value.
 */
#define FAT_ATTR_READ_ONLY 0x01 
/**
 * The corresponding file or sub-directory must 
 * not be listed unless a request is issued by the 
 * user/application explicitly requesting inclusion 
 * of "hidden files".
 */
#define FAT_ATTR_HIDDEN 0x02 
/**
 * The corresponding file is tagged as a 
 * component of the operating system.
 */
#define FAT_ATTR_SYSTEM 0x04 

/**
 * The corresponding entry contains the volume 
 * label. `firstClusterHigh` and `firstClusterLow`
 * must always be 0 for the corresponding entry 
 * (representing the volume label) since no 
 * clusters can be allocated for this entry
 * 
 * Only the root directory (see Section 6.x below) 
 * can contain one entry with this attribute.
 */
#define FAT_ATTR_VOLUME_ID 0x08 

/**
 * The corresponding entry represents a directory;
 * `fileSize` must be 0, even though clusters may 
 * have been allocated for the directory
 */
#define FAT_ATTR_DIRECTORY 0x10 

/**
 * This attribute must be set when the file is 
 *created, renamed, or modified. The presence 
 * of this attribute indicates that properties of the 
 * associated file have been modified.
 */
#define FAT_ATTR_ARCHIVE 0x20

/** Creates a fat32 file system on buf. 
 * @return 0 on success.
 */
extern int createFAT32(uint8_t *buf, const struct FATConfig *config);

/**
 * Create a directory on the fat32 disk image.
 * 
 * @param name absolute path of the directory.
 * @param buf the fat32 disk image.
 * @param flag attributes of the directory.
 * @return 0 if directory is created 
 */
extern int FAT32Mkdir(uint8_t *buf, const char *name, uint32_t flag);

/**
 * Copy a file into the disk image, at root directory.
 * It will gurantee that the file is allocated in a 
 * continuous block. One side effect of this is that
 * the offset of `fd` will be changed to the end of the file.
 *
 * @param fd the file descriptor of an opened file.
 * @param name the name of the file.
 * @return the start sector of the file, 0 on error.
 */
extern uint32_t FAT32Copyin(uint8_t *buf, int fd, const char *name, 
			        	uint32_t flag);

/**< Encode a valid FAT32 Date(System endianess). */
extern uint16_t FAT32Date(uint16_t year, uint16_t month, uint16_t day);

/**< Encode a valid FAT32 Time(System endianness). */
extern uint16_t FAT32Time(uint16_t hour, uint16_t minute, uint16_t second);

/**< An errnor number indicating what's wrong. */
extern int FAT32Errno;

#endif // FAT_H
