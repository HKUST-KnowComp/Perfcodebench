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
    
    if (row[15] == '\0' && row[2] == '-') {
      unsigned int x = 0;
      int valid = 1;
      
      for (int j = 3; j < 7; ++j) {
        if (row[j] < '0' || row[j] > '9') {
          valid = 0;
          break;
        }
        x = x * 10 + (row[j] - '0');
      }
      
      if (valid && row[7] == '-') {
        unsigned int y = 0;
        for (int j = 8; j < 15; ++j) {
          int digit = parse_hex_digit(row[j]);
          if (digit < 0) {
            valid = 0;
            break;
          }
          y = (y << 4) | digit;
        }
        
        if (valid) {
          v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
        }
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}