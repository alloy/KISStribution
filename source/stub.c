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
  printf("Looking for `__DATA,%s' section.\n", segname);
  const uint8_t *data = getsectiondata(&_mh_execute_header, "__DATA", segname, size);
  printf("Found `%lu' bytes of `__DATA,%s' data.\n", *size, segname);
  return data;
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
    unsigned long input_size = size;
    const uint8_t *lz4_data_size = archive_data("__lz4_size", &size);
    if (size == 0) {
      fprintf(stderr, "[!] No archive size data found in executable __DATA " \
                      "segment. Add a `__lz4_size' section.\n");
      return 1;
    }
    int unpacked_size = atoi((const char *)lz4_data_size);
    printf("ARCHIVE UNPACKED SIZE: %d\n", unpacked_size);

    // TODO
    /*char unpacked_data[unpacked_size];*/
    /*[>int res = LZ4_decompress_fast((const char *)lz4_data, unpacked_data, unpacked_size);<]*/
    /*int res = LZ4_decompress_safe((const char *)lz4_data, unpacked_data, input_size, unpacked_size);*/
    /*printf("RESULT: %d\n", res);*/
  }

  if (untar_data((const char *)tar_data) != 0) {
    fprintf(stderr, "[!] Failed to unpack data.\n");
    return 1;
  }

  const uint8_t *exec_cmd = archive_data("__exec_cmd", &size);
  if (size == 0) {
    fprintf(stderr, "[!] No command to execute found in executable __DATA " \
                    "segment. Add a `__exec_cmd' section.\n");
    return 1;
  }

  // TODO actually use exec_cmd
  printf("Exec: %s\n", exec_cmd);

  // TODO fork
  execl("/bin/ls", "ls", "-l", "/tmp/KISStribution.XXXXX", (char *)0);

  return 0;
}
