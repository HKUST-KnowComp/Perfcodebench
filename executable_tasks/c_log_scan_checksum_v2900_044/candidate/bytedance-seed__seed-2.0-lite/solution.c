#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t v = 0;
    // Replicate original validation logic with no libc overhead
    if (r[15] == '\0' && r[6] == '-') {
      int valid = 1;
      // Verify 4 decimal digits at positions 2-5 (matches %4u)
      for (int j = 2; j <= 5; j++) {
        if (r[j] < '0' || r[j] > '9') { valid = 0; break; }
      }
      if (valid) {
        // Verify 8 hex digits at positions7-14 (matches %8X)
        for (int j =7; j <=14; j++) {
          const char c = r[j];
          int is_hex = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
          if (!is_hex) { valid = 0; break; }
        }
      }
      if (valid) {
        v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
      }
    }
    // Exact same hash update logic as original
    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }
  return hash;
}