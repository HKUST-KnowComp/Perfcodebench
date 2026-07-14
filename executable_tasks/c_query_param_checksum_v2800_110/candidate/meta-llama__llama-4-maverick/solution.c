#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    // Skipping "x=" (2 chars), then parsing u
    int u = 0;
    row += 2;
    while (*row != '&') {
      u = u * 10 + (*row - '0');
      row++;
    }
    // Skipping "&sort=" (6 chars), then parsing s
    row += 6;
    int s = 0;
    while (*row != '&') {
      s = s * 10 + (*row - '0');
      row++;
    }
    // Skipping "&from=" (5 chars), then parsing f
    row += 5;
    int f = 0;
    while (*row != '\0' && *row != '&') {
      f = f * 10 + (*row - '0');
      row++;
    }
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
