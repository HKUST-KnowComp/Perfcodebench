#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    if (strlen(rows[i]) != 15) continue;

    int valid = 1;

    // Check digits at positions 2-5
    for (int j = 2; j < 6; ++j) {
      if (rows[i][j] < '0' || rows[i][j] > '9') {
        valid = 0;
        break;
      }
    }
    if (!valid) continue;

    // Check hyphen at position 6
    if (rows[i][6] != '-') {
      valid = 0;
    }
    if (!valid) continue;

    // Check hex digits at positions 7-14
    for (int j = 7; j < 15; ++j) {
      char c = rows[i][j];
      if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
        valid = 0;
        break;
      }
    }
    if (!valid) continue;

    uint64_t v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}