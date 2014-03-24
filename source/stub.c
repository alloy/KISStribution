#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>

#include "untar.h"
#include "lz4/lz4io.h"

static const uint8_t *
archive_data(const char *segname, int validate)
{
  printf("Looking for `__DATA,%s' section.\n", segname);
  unsigned long size = 0;
  const uint8_t *data = getsectiondata(&_mh_execute_header, "__DATA", segname, &size);
  printf("Found `%lu' bytes of `__DATA,%s' data.\n", size, segname);
  if (validate && size == 0) {
    fprintf(stderr, "[!] No `__DATA,%s' section found in executable.\n", segname);
    exit(1);
  }
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
  const uint8_t *tar_data = archive_data("__tar_data", 0);
  if (tar_data == NULL) {
    // Try to unpack LZ4 data.
    const uint8_t *lz4_data = archive_data("__lz4_data", 1);
    const uint8_t *lz4_data_size = archive_data("__lz4_size", 1);
    int unpacked_size = atoi((const char *)lz4_data_size);
    char unpacked_data[unpacked_size];
    int res = LZ4IO_decompress((const char *)lz4_data, (char **)&unpacked_data);
    if (res != 0) {
      fprintf(stderr, "[!] Unable to extract compressed lz4 data.\n");
      return 1;
    }
    tar_data = (const uint8_t *)unpacked_data;
  }

  if (untar_data((const char *)tar_data) != 0) {
    fprintf(stderr, "[!] Failed to unpack data.\n");
    return 1;
  }

  const uint8_t *exec_cmd = archive_data("__exec_cmd", 1);
  // TODO actually use exec_cmd
  printf("Exec: %s\n", exec_cmd);

  // TODO fork
  execl("/bin/ls", "ls", "-l", "/tmp/KISStribution.XXXXX", (char *)0);

  return 0;
}
