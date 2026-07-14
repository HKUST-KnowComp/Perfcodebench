#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  #define IS_VALID_HEX(c) (((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
  #define IS_VALID_DIGIT(c) (c >= '0' && c <= '9')

  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;

    if (row[15] == '\0') {
      const char d0 = row[2], d1 = row[3], d2 = row[4], d3 = row[5];
      const char hyphen = row[6];
      const char c7 = row[7], c8 = row[8], c9 = row[9], c10 = row[10];
      const char c11 = row[11], c12 = row[12], c13 = row[13], c14 = row[14];

      if (hyphen == '-' &&
          IS_VALID_DIGIT(d0) && IS_VALID_DIGIT(d1) && IS_VALID_DIGIT(d2) && IS_VALID_DIGIT(d3) &&
          IS_VALID_HEX(c7) && IS_VALID_HEX(c8) && IS_VALID_HEX(c9) && IS_VALID_HEX(c10) &&
          IS_VALID_HEX(c11) && IS_VALID_HEX(c12) && IS_VALID_HEX(c13) && IS_VALID_HEX(c14)) {
        v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
      }
    }

    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }
  #undef IS_VALID_HEX
  #undef IS_VALID_DIGIT
  return hash;
}