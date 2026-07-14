#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    // Validate exact string length matches original strlen(row) ==15 check
    if (row[15] == '\0') {
      int valid = (row[6] == '-'); // Verify mandatory hyphen in fixed format
      // Validate 4 decimal digits for original %4u parse
      valid &= (row[2] >= '0' && row[2] <= '9');
      valid &= (row[3] >= '0' && row[3] <= '9');
      valid &= (row[4] >= '0' && row[4] <= '9');
      valid &= (row[5] >= '0' && row[5] <= '9');
      // Validate 8 hex digits for original %8X parse
      #define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
      valid &= IS_HEX(row[7]);
      valid &= IS_HEX(row[8]);
      valid &= IS_HEX(row[9]);
      valid &= IS_HEX(row[10]);
      valid &= IS_HEX(row[11]);
      valid &= IS_HEX(row[12]);
      valid &= IS_HEX(row[13]);
      valid &= IS_HEX(row[14]);
      #undef IS_HEX
      if (valid) {
        v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
      }
    }
    // Preserve original FNV-1a hash calculation exactly
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }
  return hash;
}