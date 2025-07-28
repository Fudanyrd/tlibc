#ifndef ENDIAN_H
#define ENDIAN_H 1

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

static inline bool __little_endian() {
  int a = 0x1;
  return *(char *)(&a) == ((char)0x1);
}

static inline uint16_t __swap_16(const void *val) {
  uint16_t ret = 0;
  const char *src = (const char *) val;
  char *dst = (char *) &ret;

  dst[0] = src[1];
  dst[1] = src[0];

  return ret;
}

static inline uint32_t __swap_32(const void *val) {
  uint32_t ret = 0;
  const char *src = val;
  char *dst = (char *) &ret;

  dst[0] = src[3];
  dst[3] = src[0];
  dst[1] = src[2];
  dst[2] = src[1];
  return ret;
}

static inline uint64_t __swap_64(const void *val) {
  uint64_t ret;
  const char *src = val;
  char *dst = (char *) &ret;

  for (int i = 0; i < 8; i++) {
    dst[i] = src[7 - i];
  }

  return ret;
}

static inline uint16_t __load_le16(const void *src) {
  const uint16_t *s = src;
  return __little_endian() ? *s : __swap_16(s);
}

static inline void __store_le16(void *dst, uint16_t val) {
  uint16_t *d = dst;
  if (__little_endian()) {
    *d = val;
  } else {
    *d = __swap_16(&val);
  }
}

static inline uint32_t __load_le32(const void *src) {
  const uint32_t *s = src;
  return __little_endian() ? *s : __swap_32(s);
}

static inline void __store_le32(void *dst, uint32_t val) {
  uint32_t *d = dst;
  if (__little_endian()) {
    *d = val;
  } else {
    *d = __swap_32(&val);
  }
}

static inline uint64_t __load_le64(const void *src) {
  const uint64_t *s = src;
  return __little_endian() ? *s : __swap_64(s);
}

static inline void __store_le64(void *dst, uint64_t val) {
  uint64_t *d = dst;
  if (__little_endian()) {
    *d = val;
  } else {
    *d = __swap_64(&val);
  }
}

#define _generic_store_le(attr, val) \
  do {\
    switch (sizeof(attr)) {\
      case (1ul): { (attr) = (val); break; } \
      case (2ul): { __store_le16(&(attr), (val)); break; } \
      case (4ul): { __store_le32(&(attr), (val)); break; } \
      case (8ul): { __store_le64(&(attr), (val)); break; } \
      default: { assert(0 && "incorrect use of _generic_store_le"); } \
    } \
  } while(0)

/**< Load little-endian `attr` into `var`. */
#define _generic_load_le(var, attr) \
  do {\
    switch (sizeof(attr)) {\
      case (1ul): { (var) = (attr); break; } \
      case (2ul): { (var) = __load_le16(&(attr)); break; } \
      case (4ul): { (var) = __load_le32(&(attr)); break; } \
      case (8ul): { (var) = __load_le64(&(attr)); break; } \
      default: { assert(0 && "incorrect use of _generic_load_le"); } \
    } \
  } while (0)

#endif // ENDIAN_H
