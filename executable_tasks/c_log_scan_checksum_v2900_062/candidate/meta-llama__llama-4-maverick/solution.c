#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (row[0] && row[1] && row[2] && row[3] && row[4] == '-' && row[5] && row[6] && row[7] && row[8] && row[9] && row[10] && row[11] && row[12] && row[13] && row[14] && !row[15]) {
      unsigned char c2 = (unsigned char)row[2];
      unsigned char c7 = (unsigned char)row[7];
      uint64_t v = c2 + c7;
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
