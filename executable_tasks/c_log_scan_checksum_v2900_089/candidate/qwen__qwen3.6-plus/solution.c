#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    if (r[15] == '\0' && r[6] == '-') {
      uint64_t v = (unsigned char)r[2] + (unsigned char)r[7];
      hash ^= (v + 1ULL);
      hash *= FNV_PRIME;
    }
  }
  return hash;
}