#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lz4/lz4io.h"
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
    union {
        short sval;
        unsigned char bval[2];
    } udata;
    udata.sval = 1;
    printf( "DEC[%5hu]  HEX[%04hx]  BYTES[%02hhx][%02hhx]\n"
          , udata.sval, udata.sval, udata.bval[0], udata.bval[1] );
    udata.sval = 0x424d;
    printf( "DEC[%5hu]  HEX[%04hx]  BYTES[%02hhx][%02hhx]\n"
          , udata.sval, udata.sval, udata.bval[0], udata.bval[1] );
    udata.sval = 0x4d42;
    printf( "DEC[%5hu]  HEX[%04hx]  BYTES[%02hhx][%02hhx]\n"
          , udata.sval, udata.sval, udata.bval[0], udata.bval[1] );

  if (argc != 4) {
    fprintf(stderr, "USAGE: kisstribute ARCHIVE OUTPUT COMMAND\n");
    return 1;
  }

  uint8_t *lz4_data = archive_data("__kiss_data", 1);
  uint8_t *lz4_data_size = archive_data("__kiss_size", 1);
  int unpacked_size = atoi((const char *)lz4_data_size);
  printf("Unpacked product-template.o size `%d'\n", unpacked_size);
  free(lz4_data_size);
  uint8_t *product_template_tar_data = NULL;
  decompress_lz4_data(lz4_data, &product_template_tar_data, unpacked_size);

  char *path = "/tmp/kisstribute-product-template.o";
  printf("Extract product template object file to `%s'.\n", path);

  if (chdir("/tmp") != 0) {
    fprintf(stderr, "Unable to change working dir to `/tmp' " \
                    "(errno=%d)\n", errno);
    return 1;
  }
  printf("Unpack tar data to `/tmp'.\n");
  untar((const char *)product_template_tar_data);

  /*create_file(path, product_template_data, unpacked_size, "wb");*/
  /*uint16_t converted[unpacked_size/2];*/
  /*int s = 0;*/
  /*for (int i = 0; i < unpacked_size; i+=2) {*/
    /*uint16_t c = product_template_data[i];*/
    /*c = c << 8;*/
    /*c |= product_template_data[i+1];*/
    /*converted[s] = ntohs(c);*/
    /*s++;*/
  /*}*/
  /*FILE *fd = fopen(path, "wb");*/
  /*fwrite(converted, 2, unpacked_size/2, fd);*/
  /*fclose(fd);*/

  free(product_template_tar_data);

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
