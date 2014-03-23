#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>

static const uint8_t *
archive_data(unsigned long *size)
{
  return getsectiondata(&_mh_execute_header, "__DATA", "__tar_data", size);
}

int
main() {
  unsigned long size = 0;
  const uint8_t *data = archive_data(&size);
  if (size == 0) {
    printf("[!] No data found!\n");
  } else {
    /*printf("SIZE: %lu. DATA: %s\n", size, (unsigned char *)data);*/

    /*char *root = mkdtemp("/tmp/KISStribution.XXXXX");*/
    char *root = "/tmp/KISStribution.XXXXX";
    printf("CREATE AND CHANGE TO ROOT: %s\n", root);
    if (mkdir(root, 0755) != 0 && errno != EEXIST) {
      fprintf(stderr, "Unable to create root at `%s' (errno=%d)\n", root, errno);
      return 1;
    }

    if (chdir(root) != 0) {
      fprintf(stderr, "Unable to change working dir to root at `%s' (errno=%d)\n", root, errno);
      return 1;
    }

    void untar(const char *input);
    untar((const char *)data);
  }
  return 0;
}
