#include "interface.h"
#include <stdint.h>
#include <string.h>

// Custom fast parser for fixed-format log rows
uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (strlen(row) != 15) continue;

    char p0 = row[0];
    char p1 = row[1];
    unsigned int x = 0;
    unsigned int y = 0;

    // Manually parse integers from ASCII
    for (int j = 0; j < 4; ++j) {
      char c = row[j + 2];
      if (c < '0' || c > '9') break;
      x = x * 10 + (c - '0');
    }

    for (int j = 0; j < 8; ++j) {
      char c = row[j + 6];
      if (c >= '0' && c <= '9') {
        y = y * 16 + (c - '0');
      } else if (c >= 'A' && c <= 'F') {
        y = y * 16 + (c - 'A' + 10);
      } else if (c >= 'a' && c <= 'f') {
        y = y * 16 + (c - 'a' + 10);
      } else {
        break;
      }
    }

    uint64_t v = 0;
    if (p0 != 0 && p1 != 0 && x > 0 && y > 0) {
      v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}