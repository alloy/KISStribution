#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>

#include "lz4/lz4.h"

static const uint8_t *
archive_data(const char *segname, unsigned long *size)
{
  printf("Looking for `__DATA' section `%s'.\n", segname);
  return getsectiondata(&_mh_execute_header, "__DATA", segname, size);
}

void
untar(const char *);

static int
untar_data(const char *data)
{
  /*char *root = mkdtemp("/tmp/KISStribution.XXXXX");*/
  char *root = "/tmp/KISStribution.XXXXX";
  printf("Create prefix root at `%s'.\n", root);
  if (mkdir(root, 0755) != 0 && errno != EEXIST) {
    fprintf(stderr, "Unable to create root at `%s' (errno=%d)\n", root, errno);
    return 1;
  }

  printf("Change to prefix root at `%s'.\n", root);
  if (chdir(root) != 0) {
    fprintf(stderr, "Unable to change working dir to root at `%s' " \
                    "(errno=%d)\n", root, errno);
    return 1;
  }

  printf("Unpack tar data to prefix root at `%s'.\n", root);
  untar(data);

  return 0;
}

int
main() {
  unsigned long size = 0;
  const uint8_t *tar_data = archive_data("__tar_data", &size);
  if (size == 0) {
    const uint8_t *lz4_data = archive_data("__lz4_data", &size);
    if (size == 0) {
      fprintf(stderr, "[!] No data found in executable __DATA segment. Add " \
                      "either a `__tar_data' or a `__lz4_data' section.\n");
      return 1;
    }
  }

  return untar_data((const char *)tar_data);
}
