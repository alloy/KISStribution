#include <stdint.h>

// Needs to be freed.
uint8_t *
archive_data(const char *segname, int validate);

// Needs to be freed.
void
decompress_lz4_data(uint8_t *lz4_data, uint8_t **out, int unpacked_size);
