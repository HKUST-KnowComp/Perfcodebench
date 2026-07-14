#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (row[2] >= '0' && row[2] <= '9' && row[6] == '-' && row[15] == '\0') {
      uint64_t v = (unsigned char)row[2] + (unsigned char)row[7];
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
