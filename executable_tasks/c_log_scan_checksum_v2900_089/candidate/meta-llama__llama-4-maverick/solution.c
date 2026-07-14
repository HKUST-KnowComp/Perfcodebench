#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (row[2] == '-' && row[7] == '-' && row[11] == ' ' &&
        row[0] >= 0 && row[1] >= 0 &&
        (unsigned char)row[3] >= '0' && (unsigned char)row[3] <= '9' &&
        (unsigned char)row[4] >= '0' && (unsigned char)row[4] <= '9' &&
        (unsigned char)row[5] >= '0' && (unsigned char)row[5] <= '9' &&
        (unsigned char)row[6] >= '0' && (unsigned char)row[6] <= '9' &&
        (unsigned char)row[8] >= '0' && (unsigned char)row[8] <= '9' &&
        (unsigned char)row[9] >= '0' && (unsigned char)row[9] <= '9' &&
        (unsigned char)row[10] >= '0' && (unsigned char)row[10] <= '9' &&
        (unsigned char)row[12] >= '0' && (unsigned char)row[12] <= '9' &&
        (unsigned char)row[13] >= '0' && (unsigned char)row[13] <= '9' &&
        (unsigned char)row[14] >= '0' && (unsigned char)row[14] <= '9' &&
        (unsigned char)row[15] >= '0' && (unsigned char)row[15] <= '9' &&
        row[16] == '\0') {
      uint64_t v = (unsigned char)row[2] + (unsigned char)row[7];
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
