#include "archive_data.h"
#include "lz4/lz4io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>

uint8_t *
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

void
decompress_lz4_data(uint8_t *lz4_data, uint8_t **out, int unpacked_size)
{
  char unpacked_data[unpacked_size];
  int res = LZ4IO_decompress((const char *)lz4_data, (char **)&unpacked_data);
  free(lz4_data);
  if (res != 0) {
    fprintf(stderr, "[!] Unable to extract compressed lz4 data.\n");
    exit(1);
  }
  *out = malloc(sizeof(uint8_t) * unpacked_size);
  memcpy(*out, unpacked_data, unpacked_size);
}

