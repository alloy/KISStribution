#include "string_io.h"
#include <string.h>

size_t
sread(void *ptr, size_t size, size_t nitems, char **source)
{
  size_t bytes = size * nitems;
  memcpy(ptr, *source, bytes);
  *source += bytes;
  return nitems;
}

