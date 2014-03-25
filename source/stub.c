#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>

#include "untar.h"
#include "lz4/lz4io.h"

// Needs to be freed.
static uint8_t *
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
  if (size == 0) {
    return NULL;
  }
  uint8_t *result = malloc(sizeof(uint8_t) * (size + 1));
  memcpy(result, data, size);
  result[size] = '\0';
  return result;
}

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

static void
unpack_data(void)
{
  uint8_t *tar_data = archive_data("__tar_data", 0);
  if (tar_data == NULL) {
    // Try to unpack LZ4 data.
    uint8_t *lz4_data = archive_data("__lz4_data", 1);
    uint8_t *lz4_data_size = archive_data("__lz4_size", 1);
    int unpacked_size = atoi((const char *)lz4_data_size);
    free(lz4_data_size);
    char unpacked_data[unpacked_size];
    int res = LZ4IO_decompress((const char *)lz4_data, (char **)&unpacked_data);
    free(lz4_data);
    if (res != 0) {
      fprintf(stderr, "[!] Unable to extract compressed lz4 data.\n");
      exit(1);
    }
    tar_data = malloc(sizeof(uint8_t) * unpacked_size);
    memcpy(tar_data, unpacked_data, unpacked_size);
  }

  int res = untar_data((const char *)tar_data);
  free(tar_data);
  if (res != 0) {
    fprintf(stderr, "[!] Failed to unpack data.\n");
    exit(1);
  }
}

// Needs to be freed.
//
// TODO this is not shell-split safe, need to come up with a good way to
// encode and preserve white space where necessary.
static char **
shellsplit(const char *input)
{
  char **components = NULL;
  char *p = strtok((char *)input, " ");
  int components_count = 0;
  while (p) {
    components = realloc(components, sizeof(char *) * (++components_count));
    assert(components != NULL && "Unable to allocate memory.");
    components[components_count-1] = p;
    p = strtok(NULL, " ");
  }
  components = realloc(components, sizeof(char *) * (++components_count));
  components[components_count-1] = NULL;
  return components;
}

int
main() {
  unpack_data();

  uint8_t *exec_cmd = archive_data("__exec_cmd", 1);
  printf("Exec: %s\n", exec_cmd);
  char **components = shellsplit((const char *)exec_cmd);
  free(exec_cmd);

  // TODO fork
  int status = execv(components[0], &components[0]);
  free(components);

  return status;
}
