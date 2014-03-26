#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "archive_data.h"
#include "untar.h"

static void
create_file(char *path, uint8_t *data, size_t size, const char *mode)
{
  FILE *fd = fopen(path, mode);
  fwrite(data, size, 1, fd);
  fclose(fd);
}

int
main(int argc, char **argv)
{
  if (argc != 4) {
    fprintf(stderr, "USAGE: kisstribute ARCHIVE OUTPUT COMMAND\n");
    return 1;
  }

  uint8_t *product_template_data = archive_data("__kiss_data", 1);

  char *path = "/tmp/product-template.o";
  printf("Extract product template object file to `%s'.\n", path);

  const char *wd = getwd(NULL);
  if (chdir("/tmp") != 0) {
    fprintf(stderr, "Unable to change working dir to `/tmp' " \
                    "(errno=%d)\n", errno);
    return 1;
  }
  printf("Unpack tar data to `/tmp'.\n");
  untar((const char *)product_template_data);
  free(product_template_data);
  if (chdir(wd) != 0) {
    fprintf(stderr, "Unable to change working dir to `%s' " \
                    "(errno=%d)\n", wd, errno);
    return 1;
  }

  // TODO add compress with lz4 support
  char *cmd_file = "/tmp/kisstribute-cmd";
  create_file(cmd_file, (uint8_t *)argv[3], strlen(argv[3]), "w");
  char *object_file = "/tmp/kisstribute-product.o";
  /*execl("/usr/bin/ld", "-r", path, "-sectcreate", "__DATA", "__tar_data", argv[1], "-sectcreate", "__DATA", "__exec_cmd", cmd_file, "-o", object_file);*/
  char buf[1024];
  sprintf(buf, "/usr/bin/ld -r %s -sectcreate __DATA __tar_data %s -sectcreate __DATA __exec_cmd %s -o %s", path, argv[1], cmd_file, object_file);
  printf("%s\n", buf);
  system(buf);
  /*execl("/usr/bin/clang", object_file, "-o", argv[2]);*/
  sprintf(buf, "/usr/bin/clang %s -o %s", object_file, argv[2]);
  printf("%s\n", buf);
  system(buf);

  return 0;
}
