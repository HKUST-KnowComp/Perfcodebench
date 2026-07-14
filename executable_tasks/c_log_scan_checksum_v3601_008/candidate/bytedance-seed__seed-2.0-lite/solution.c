#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    // Replicate original's strlen(row) ==15 check with O(1) null position check
    if (row[15] == '\0') {
      // Verify required literal '-' from sscanf format at fixed offset 6
      if (row[6] == '-') {
        // Validate 4 decimal digits for %4u at offsets 2-5
        int dec_valid = 1;
        for (int j = 2; j <= 5; j++) {
          if (row[j] < '0' || row[j] > '9') { dec_valid = 0; break; }
        }
        if (dec_valid) {
          // Validate 8 hex digits for %8X at offsets7-14
          int hex_valid = 1;
          for (int j = 7; j <=14; j++) {
            char c = row[j];
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
              hex_valid = 0; break;
            }
          }
          if (hex_valid) {
            // Compute v exactly as original implementation
            v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
          }
        }
      }
    }
    // Maintain identical FNV-1a hash computation
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }
  return hash;
}