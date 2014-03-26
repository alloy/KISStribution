#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "archive_data.h"
#include "untar.h"

static char *
prefix_path(const char *program_path)
{
  char *uid = (char *)archive_data("__uid", 1);
  char *program_name = basename((char *)program_path);
  int size = strlen(program_name) + strlen(uid) + 7;
  char *path = malloc(sizeof(char) * size);
  snprintf(path, size, "/tmp/%s-%s", program_name, uid);
  return path;
}

static int
cwd_to_prefix(char *prefix)
{
  printf("Create prefix root at `%s'.\n", prefix);
  if (mkdir(prefix, 0755) != 0 && errno != EEXIST) {
    fprintf(stderr, "Unable to create root at `%s' (errno=%d)\n", prefix, errno);
    return 1;
  }

  printf("Change to prefix root at `%s'.\n", prefix);
  if (chdir(prefix) != 0) {
    fprintf(stderr, "Unable to change working dir to root at `%s' " \
                    "(errno=%d)\n", prefix, errno);
    return 1;
  }

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

  printf("Unpack tar data.\n");
  untar((const char *)tar_data);
  free(tar_data);
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
main(int argc, const char **argv) {
  char *root = prefix_path(argv[0]);

  // TODO we should not change the wd as the actual command might need to perform work there.
  cwd_to_prefix(root);
  unpack_data();

  uint8_t *exec_cmd = archive_data("__exec_cmd", 1);
  printf("Exec: %s\n", exec_cmd);
  char **components = shellsplit((const char *)exec_cmd);
  free(exec_cmd);
  int i = 0;
  while (1) {
    if (components[i] == '\0') {
      break;
    }
    printf("components[%d] = %s\n", i, components[i]);
    i++;
  }

  printf("ENV: KISSTRIBUTE_PREFIX=%s\n", root);
  setenv("KISSTRIBUTE_PREFIX", root, 0);

  free(root);

  // TODO fork
  int status = execv(components[0], &components[0]);
  assert(0 && "Should not be reached");

  free(components);
  return status;
}
