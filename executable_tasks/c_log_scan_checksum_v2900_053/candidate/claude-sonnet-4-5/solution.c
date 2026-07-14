#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static inline int is_hex_digit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static inline int is_digit(char c) {
  return c >= '0' && c <= '9';
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    
    // Check fixed format: XX####-########\0 (15 chars + null)
    // Positions: 0,1 = prefix, 2-5 = 4 digits, 6 = '-', 7-14 = 8 hex digits, 15 = '\0'
    if (row[6] == '-' && row[15] == '\0' &&
        is_digit(row[2]) && is_digit(row[3]) && is_digit(row[4]) && is_digit(row[5]) &&
        is_hex_digit(row[7]) && is_hex_digit(row[8]) && is_hex_digit(row[9]) && is_hex_digit(row[10]) &&
        is_hex_digit(row[11]) && is_hex_digit(row[12]) && is_hex_digit(row[13]) && is_hex_digit(row[14])) {
      v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}