#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "archive_data.h"
#include "untar.h"

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
    uint8_t *lz4_data = archive_data("__lz4_data", 1);
    uint8_t *lz4_data_size = archive_data("__lz4_size", 1);
    int unpacked_size = atoi((const char *)lz4_data_size);
    decompress_lz4_data(lz4_data, &tar_data, unpacked_size);
    free(lz4_data_size);
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
