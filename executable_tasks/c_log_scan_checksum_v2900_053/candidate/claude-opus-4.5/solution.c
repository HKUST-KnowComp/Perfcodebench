#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t v = 0;
    
    // Check length is exactly 15 and format matches %c%c%4u-%8X
    // Format: [0][1][2-5 digits][6 dash][7][8-14 hex]
    if (r[15] == '\0' && r[6] == '-') {
      // Check positions 2-5 are digits (4-digit unsigned)
      unsigned char c2 = (unsigned char)r[2];
      unsigned char c3 = (unsigned char)r[3];
      unsigned char c4 = (unsigned char)r[4];
      unsigned char c5 = (unsigned char)r[5];
      
      int digits_ok = (c2 >= '0' && c2 <= '9') &&
                      (c3 >= '0' && c3 <= '9') &&
                      (c4 >= '0' && c4 <= '9') &&
                      (c5 >= '0' && c5 <= '9');
      
      if (digits_ok) {
        // Check positions 8-14 are hex digits (8 hex chars, but position 7 is first)
        // Actually format is %c%c%4u-%8X, so after dash we have 8 hex chars at positions 7-14
        unsigned char h0 = (unsigned char)r[7];
        unsigned char h1 = (unsigned char)r[8];
        unsigned char h2 = (unsigned char)r[9];
        unsigned char h3 = (unsigned char)r[10];
        unsigned char h4 = (unsigned char)r[11];
        unsigned char h5 = (unsigned char)r[12];
        unsigned char h6 = (unsigned char)r[13];
        unsigned char h7 = (unsigned char)r[14];
        
        #define IS_HEX(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
        
        int hex_ok = IS_HEX(h0) && IS_HEX(h1) && IS_HEX(h2) && IS_HEX(h3) &&
                     IS_HEX(h4) && IS_HEX(h5) && IS_HEX(h6) && IS_HEX(h7);
        
        #undef IS_HEX
        
        if (hex_ok) {
          v = (uint64_t)(c2 + h0);
        }
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
