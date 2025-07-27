// Credit: https://unix.stackexchange.com/questions/52215/determine-the-size-of-a-block-device
#include <stdint.h>
#include <stdio.h>

/**
 * @return the size of a block device.
 */
size_t sizeOfBlockdev(const char *name)
{
  static char buf[192];
  sprintf(buf, "/sys/class/block/%s/size", name);

  FILE *fobj = fopen(buf, "r");
  size_t ret;
  fscanf(fobj, "%ld", &ret);
  return ret;
}

/** FIXME: for testing only */
__attribute__((weak)) int main(int argc, char **argv)
{
  for (int i = 1; i < argc; i++)
  {
    printf("%s: %lx\n", argv[i], sizeOfBlockdev(argv[i]));
  }

  return 0;
}
