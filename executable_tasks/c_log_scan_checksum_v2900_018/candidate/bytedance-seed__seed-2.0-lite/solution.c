#include "interface.h"

#include <stdint.h>
#include <string.h>

#define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t v = 0;
    // Validate row matches original sscanf + strlen conditions exactly
    if (r[15] == '\0' && r[6] == '-' &&
        // Validate 4 decimal digits at positions 2-5
        (r[2] >= '0' && r[2] <= '9') &&
        (r[3] >= '0' && r[3] <= '9') &&
        (r[4] >= '0' && r[4] <= '9') &&
        (r[5] >= '0' && r[5] <= '9') &&
        // Validate 8 hex digits at positions 7-14
        IS_HEX(r[7]) && IS_HEX(r[8]) && IS_HEX(r[9]) && IS_HEX(r[10]) &&
        IS_HEX(r[11]) && IS_HEX(r[12]) && IS_HEX(r[13]) && IS_HEX(r[14])) {
      v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}