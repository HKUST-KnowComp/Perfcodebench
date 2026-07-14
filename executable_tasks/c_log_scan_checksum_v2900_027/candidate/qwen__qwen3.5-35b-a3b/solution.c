#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (strlen(row) != 15) continue;
    
    // Check 4-digit number (positions 2-5)
    int valid_digits = 1;
    for (int j = 2; j < 6; ++j) {
      if (row[j] < '0' || row[j] > '9') {
        valid_digits = 0;
        break;
      }
    }
    if (!valid_digits) continue;
    
    // Check hyphen at position 6
    if (row[6] != '-') continue;
    
    // Check 8 hex digits (positions 7-14)
    int valid_hex = 1;
    for (int j = 7; j < 15; ++j) {
      char c = row[j];
      if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
        valid_hex = 0;
        break;
      }
    }
    if (!valid_hex) continue;
    
    // Compute v
    uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}