#include "archive_data.h"

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
