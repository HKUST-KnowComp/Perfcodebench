#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t v = 0;
    
    // Fast fixed-format validation: exact length 15 and delimiter at index 6
    if (r[15] == '\0' && r[6] == '-') {
      // Verify digits at indices 2-5 (required for %4u to succeed before '-')
      if (r[2] >= '0' && r[2] <= '9' &&
          r[3] >= '0' && r[3] <= '9' &&
          r[4] >= '0' && r[4] <= '9' &&
          r[5] >= '0' && r[5] <= '9') {
        // Verify at least one hex digit at index 7 (required for %8X to succeed)
        char c7 = r[7];
        if ((c7 >= '0' && c7 <= '9') || (c7 >= 'A' && c7 <= 'F') || (c7 >= 'a' && c7 <= 'f')) {
          v = (uint64_t)((unsigned char)r[2] + (unsigned char)c7);
        }
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
