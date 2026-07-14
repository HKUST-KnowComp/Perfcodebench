#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    uint64_t v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}