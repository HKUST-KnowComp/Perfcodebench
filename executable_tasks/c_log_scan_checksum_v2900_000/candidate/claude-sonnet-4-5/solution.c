#include "interface.h"

#include <stdint.h>

static inline int parse_hex_digit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    
    // Check format: CCdddd-HHHHHHHH\0 (length 15)
    // Positions: 0,1,2-5,6,7-14,15
    if (row[6] == '-' && row[15] == '\0') {
      // Parse 4-digit decimal at positions 2-5
      char d0 = row[2], d1 = row[3], d2 = row[4], d3 = row[5];
      if (d0 >= '0' && d0 <= '9' && d1 >= '0' && d1 <= '9' &&
          d2 >= '0' && d2 <= '9' && d3 >= '0' && d3 <= '9') {
        
        // Parse 8-digit hex at positions 7-14
        int h0 = parse_hex_digit(row[7]);
        int h1 = parse_hex_digit(row[8]);
        int h2 = parse_hex_digit(row[9]);
        int h3 = parse_hex_digit(row[10]);
        int h4 = parse_hex_digit(row[11]);
        int h5 = parse_hex_digit(row[12]);
        int h6 = parse_hex_digit(row[13]);
        int h7 = parse_hex_digit(row[14]);
        
        if ((h0 | h1 | h2 | h3 | h4 | h5 | h6 | h7) >= 0) {
          v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
        }
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}
