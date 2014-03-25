#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "archive_data.h"

static void
create_file(char *path, char *data, size_t size)
{
  FILE *fd = fopen(path, "w");
  fwrite(data, 1, size, fd);
  fclose(fd);
}

int
main(int argc, char **argv)
{
  if (argc != 4) {
    fprintf(stderr, "USAGE: kisstribute ARCHIVE OUTPUT COMMAND\n");
    return 1;
  }

  uint8_t *lz4_data = archive_data("__kiss_data", 1);
  uint8_t *lz4_data_size = archive_data("__kiss_size", 1);
  int unpacked_size = atoi((const char *)lz4_data_size);
  free(lz4_data_size);
  uint8_t *product_template_data = NULL;
  decompress_lz4_data(lz4_data, &product_template_data, unpacked_size);

  char *path = "/tmp/kisstribute-product-template.o";
  printf("Extract product template object file to `%s'.\n", path);
  create_file(path, (char *)product_template_data, unpacked_size);

  free(product_template_data);

  // TODO add compress with lz4 support
  char *cmd_file = "/tmp/kisstribute-cmd";
  create_file(cmd_file, argv[3], strlen(argv[3]));
  char *object_file = "/tmp/kisstribute-product.o";
  /*execl("/usr/bin/ld", "-r", path, "-sectcreate", "__DATA", "__tar_data", argv[1], "-sectcreate", "__DATA", "__exec_cmd", cmd_file, "-o", object_file);*/
  char buf[1024];
  sprintf(buf, "/usr/bin/ld -r %s -sectcreate __DATA __tar_data %s -sectcreate __DATA __exec_cmd %s -o %s", path, argv[1], cmd_file, object_file);
  system(buf);
  /*execl("/usr/bin/clang", object_file, "-o", argv[2]);*/
  sprintf(buf, "/usr/bin/clang %s -o %s", object_file, argv[2]);
  system(buf);

  return 0;
}
