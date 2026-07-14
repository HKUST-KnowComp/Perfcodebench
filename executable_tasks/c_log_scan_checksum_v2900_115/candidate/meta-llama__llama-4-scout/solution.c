#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    char p0 = rows[i][0];
    char p1 = rows[i][1];
    unsigned int x = *(unsigned int*)(rows[i] + 2);
    unsigned int y = *(unsigned int*)(rows[i] + 6);
    if (p0 != '\0' && p1 != '\0' && x >= 1000 && x <= 9999 && y >= 0 && y <= 0xFFFFFFFF) {
      uint64_t v = (uint64_t)(rows[i][2] + rows[i][7]);
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}