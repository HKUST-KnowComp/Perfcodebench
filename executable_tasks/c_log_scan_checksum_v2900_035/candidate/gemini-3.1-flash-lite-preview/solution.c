#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    
    // Fixed format: 2 chars, 4 digits, '-', 8 hex chars, null terminator (15 chars total)
    // Check length implicitly by verifying the structure at specific offsets
    if (s[0] && s[1] && 
        s[2] >= '0' && s[2] <= '9' &&
        s[3] >= '0' && s[3] <= '9' &&
        s[4] >= '0' && s[4] <= '9' &&
        s[5] >= '0' && s[5] <= '9' &&
        s[6] == '-' &&
        s[7] != '\0' && s[8] != '\0' && s[9] != '\0' && s[10] != '\0' &&
        s[11] != '\0' && s[12] != '\0' && s[13] != '\0' && s[14] != '\0' &&
        s[15] == '\0') {
      v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}