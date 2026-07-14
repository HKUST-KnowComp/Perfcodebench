#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    
    // Format: XX####-########\0 (15 chars + null)
    // Validate fixed positions and format
    if (row[2] >= '0' && row[2] <= '9' &&
        row[3] >= '0' && row[3] <= '9' &&
        row[4] >= '0' && row[4] <= '9' &&
        row[5] >= '0' && row[5] <= '9' &&
        row[6] == '-' &&
        ((row[7] >= '0' && row[7] <= '9') || (row[7] >= 'A' && row[7] <= 'F') || (row[7] >= 'a' && row[7] <= 'f')) &&
        ((row[8] >= '0' && row[8] <= '9') || (row[8] >= 'A' && row[8] <= 'F') || (row[8] >= 'a' && row[8] <= 'f')) &&
        ((row[9] >= '0' && row[9] <= '9') || (row[9] >= 'A' && row[9] <= 'F') || (row[9] >= 'a' && row[9] <= 'f')) &&
        ((row[10] >= '0' && row[10] <= '9') || (row[10] >= 'A' && row[10] <= 'F') || (row[10] >= 'a' && row[10] <= 'f')) &&
        ((row[11] >= '0' && row[11] <= '9') || (row[11] >= 'A' && row[11] <= 'F') || (row[11] >= 'a' && row[11] <= 'f')) &&
        ((row[12] >= '0' && row[12] <= '9') || (row[12] >= 'A' && row[12] <= 'F') || (row[12] >= 'a' && row[12] <= 'f')) &&
        ((row[13] >= '0' && row[13] <= '9') || (row[13] >= 'A' && row[13] <= 'F') || (row[13] >= 'a' && row[13] <= 'f')) &&
        ((row[14] >= '0' && row[14] <= '9') || (row[14] >= 'A' && row[14] <= 'F') || (row[14] >= 'a' && row[14] <= 'f')) &&
        row[15] == '\0') {
      v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}