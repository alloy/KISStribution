#include <stdlib.h>

int
sseek(char **source, size_t offset, int whence);

long
stell(char **source);

size_t
sread(void *ptr, size_t size, size_t nitems, char **source);

size_t
swrite(const void *ptr, size_t size, size_t nitems, char **destination);
