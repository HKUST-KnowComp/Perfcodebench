#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (row[0] != '\0' && row[1] != '\0' && row[2] != '\0' && row[3] != '-' &&
        row[4] != '\0' && row[5] != '\0' && row[6] != '\0' && row[7] != '\0' &&
        row[8] != '\0' && row[9] != '\0' && row[10] != '\0' && row[11] != '\0' &&
        row[12] != '\0' && row[13] != '\0' && row[14] == '\0') {
      uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
