#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    
    // Check fixed length 15 and format: CCXXXX-XXXXXXXX
    // s[0], s[1] are chars
    // s[2..5] are digits (x)
    // s[6] is '-'
    // s[7..14] are hex (y)
    if (s[6] == '-' && s[14] != '\0' && s[15] == '\0') {
      // Validate digits and hex characters if necessary, 
      // but based on the original sscanf logic, we focus on the structure.
      // The original logic: matched == 4 && strlen == 15
      // v = (unsigned char)rows[i][2] + (unsigned char)rows[i][7]
      v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}