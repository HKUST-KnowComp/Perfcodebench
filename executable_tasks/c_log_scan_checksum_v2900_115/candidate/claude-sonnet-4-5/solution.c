#include "interface.h"

#include <stdint.h>

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
    
    // Format: XX####-YYYYYYYY (15 chars total)
    // Check length by verifying position 15 is null and position 14 is not
    if (row[15] == '\0' && row[14] != '\0' &&
        row[2] >= '0' && row[2] <= '9' &&
        row[3] >= '0' && row[3] <= '9' &&
        row[4] >= '0' && row[4] <= '9' &&
        row[5] >= '0' && row[5] <= '9' &&
        row[6] == '-' &&
        is_hex_digit(row[7]) && is_hex_digit(row[8]) &&
        is_hex_digit(row[9]) && is_hex_digit(row[10]) &&
        is_hex_digit(row[11]) && is_hex_digit(row[12]) &&
        is_hex_digit(row[13]) && is_hex_digit(row[14])) {
      v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}