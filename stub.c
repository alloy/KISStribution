#include <stdlib.h>
#include <stdio.h>

#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>

static const uint8_t *
archive_data(unsigned long *size)
{
  return getsectiondata(&_mh_execute_header, "__DATA", "__tar_data", size);
}

int
main() {
  unsigned long size = 0;
  const uint8_t *data = archive_data(&size);
  if (size == 0) {
    printf("[!] No data found!\n");
  } else {
    /*printf("SIZE: %lu. DATA: %s\n", size, (unsigned char *)data);*/

    void untar(const char *input);
    untar((const char *)data);
  }
  exit(0);
}
