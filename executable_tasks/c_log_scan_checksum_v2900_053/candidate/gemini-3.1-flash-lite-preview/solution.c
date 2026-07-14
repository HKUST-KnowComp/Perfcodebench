#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    
    // Fixed format: 2 chars, 4 digits, '-', 8 hex chars, null terminator (total 15 chars)
    // Check length implicitly by verifying the character at index 14 is 0
    // and index 6 is '-'.
    if (s[6] == '-' && s[14] == '\0') {
      // Validate digits 2-5 are numeric
      if (s[2] >= '0' && s[2] <= '9' && s[3] >= '0' && s[3] <= '9' &&
          s[4] >= '0' && s[4] <= '9' && s[5] >= '0' && s[5] <= '9') {
        
        // Validate hex chars 7-14
        int valid_hex = 1;
        for (int j = 7; j < 14; ++j) {
          char c = s[j];
          if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            valid_hex = 0;
            break;
          }
        }

        if (valid_hex) {
          v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
        }
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}