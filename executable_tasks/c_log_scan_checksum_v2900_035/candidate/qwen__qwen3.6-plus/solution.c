#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t v = 0;
    if (r[15] == '\0' && r[6] == '-') {
      v = (unsigned char)r[2] + (unsigned char)r[7];
    }
    hash ^= (v + 1ULL);
    hash *= prime;
  }
  return hash;
}
