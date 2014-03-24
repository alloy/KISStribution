#include "string_io.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int
sseek(char **source, size_t offset, int whence)
{
  assert(whence == SEEK_CUR && "`sseek` only supports SEEK_CUR");
  *source += offset;
  return 0;
}

long
stell(char **source)
{
  return *source[0];
}

size_t
sread(void *ptr, size_t size, size_t nitems, char **source)
{
  size_t bytes = size * nitems;
  memcpy(ptr, *source, bytes);
  *source += bytes;
  return nitems;
}

size_t
swrite(const void *ptr, size_t size, size_t nitems, char **destination)
{
  size_t bytes = size * nitems;
  memcpy(destination, &ptr, bytes);
  *destination += bytes;
  return nitems;
}
