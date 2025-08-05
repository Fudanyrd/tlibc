// extract the payload (ususally gzip compressed ELF)
// from a bzImage.
//
// usage:
// getpayload input_file [output_file]
//

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define __bi_debug(fmt, ...) \
  do { fprintf(stderr, fmt, ## __VA_ARGS__); } while (0)

#include "bz_image.h"

/**< verify that Kernel header is valid. */
static int BiCheck(const struct KernelHeader *khdr) {
  if (khdr->boot_flag != 0xaa55) {
    __bi_debug("Incorrect boot_flag, should be: 0xaa55, is %x\n", 
               khdr->boot_flag);
    return 1;
  }

  if (khdr->header != 0x53726448) {
    __bi_debug("Incorrect header, should be: HdrS, is: %x\n", khdr->header);
    return 1;
  }

  return 0;
}

static const void *BiGetPayload(const void *filedata,
  size_t *size) {
  const struct KernelHeader *khdr = filedata;
  if (BiCheck(khdr) != 0) {
    __bi_debug("I cannot read the header. Abort\n");
    return NULL;
  }

  uint8_t skip_sects = khdr->setup_sects;
  skip_sects = skip_sects == 0 ? 4u : skip_sects;
  skip_sects += 1;
  //
  // these skipped sectors (`skip_sects`)
  // stores the kernel boot sector and 
  // setup code; the next part is the
  // "protected-mode" code.
  //

  size_t payload_len = khdr->payload_length;
  size_t offset = khdr->payload_offset;
  offset += LINUX_SECTOR_SIZE * skip_sects;

  *size = payload_len;
  return (filedata + offset);
}

__attribute__((weak))
int main(int argc, char **argv) {
  // parse arguments.
  // default output is 
  if (argc != 2 && argc != 3) {
    __bi_debug("usage: getpayload input_file [output_file]\n");
    return 1;
  }
  const char *input = argv[1];
  const char *output = argv[2];
  output = output == NULL ? "a.img" : output;

  int fd = open(input, O_RDONLY);
  if (fd < 0) {
    __bi_debug("cannot open %s\n", input);
    return 1;
  }

  // stat and get file size, round up to 
  // a multiple of page size.
  size_t file_size;
  do {
    struct stat st;
    if (fstat(fd, &st) != 0) {
      __bi_debug("cannot open %s\n", input);
      return 1;
    }
    file_size = st.st_size;
    file_size = (file_size + 4095u) / 4096u * 4096u;
  } while (0);

  void *filedata = mmap(NULL, file_size, PROT_READ, 
    MAP_PRIVATE, fd, 0);
  if (filedata == ((const void *) -1)) {
    __bi_debug("cannot open %s\n", input);
    return 1;
  }

  size_t len;
  const void *addr = BiGetPayload(filedata, &len);
  if (addr == NULL) {
    return 1;
  }

  int dstfd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0777);  
  if (dstfd < 0 || write(dstfd, addr, len) != len) {
    munmap(filedata, file_size);
    close(fd);
    __bi_debug("fail to write to output %s\n", output);
    return 1;
  }

  munmap(filedata, file_size);
  close(dstfd);
  close(fd);
  return 0;
}
