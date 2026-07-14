#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t val = 1;
    if (r[6] == '-' && r[15] == '\0') {
      val += (unsigned char)r[2] + (unsigned char)r[7];
    }
    hash ^= val;
    hash *= 1099511628211ULL;
  }
  return hash;
}