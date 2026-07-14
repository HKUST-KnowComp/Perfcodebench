#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    
    // Format: XX####-XXXXXXXX (15 chars total)
    // Check length implicitly via null terminator at position 15
    // Check dash at position 6
    if (row[6] == '-' && row[15] == '\0') {
      // Validate positions 2-5 are digits
      if ((row[2] >= '0' && row[2] <= '9') &&
          (row[3] >= '0' && row[3] <= '9') &&
          (row[4] >= '0' && row[4] <= '9') &&
          (row[5] >= '0' && row[5] <= '9')) {
        // Validate positions 7-14 are hex digits
        int valid_hex = 1;
        for (int j = 7; j <= 14; ++j) {
          char c = row[j];
          if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            valid_hex = 0;
            break;
          }
        }
        if (valid_hex) {
          v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
        }
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}