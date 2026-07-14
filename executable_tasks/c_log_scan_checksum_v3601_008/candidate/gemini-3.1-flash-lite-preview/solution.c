#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    
    /* 
     * Expected format: XXDDDD-XXXXXXXX (15 chars)
     * Indices: 0,1 (chars), 2,3,4,5 (digits), 6 (-), 7,8,9,10,11,12,13,14 (hex)
     * Check: s[6] == '-' and s[14] is the last char (length 15)
     */
    if (s[6] == '-' && s[14] != '\0' && s[15] == '\0') {
      int valid = 1;
      for (int j = 2; j <= 5; ++j) if (s[j] < '0' || s[j] > '9') valid = 0;
      for (int j = 7; j <= 14; ++j) {
        char c = s[j];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
          valid = 0;
          break;
        }
      }
      
      if (valid) {
        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}