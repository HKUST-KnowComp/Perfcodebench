#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    char p0 = 0;
    char p1 = 0;
    unsigned int x = 0;
    unsigned int y = 0;
    if (sscanf(rows[i], "%c%c%4u-%8X", &p0, &p1, &x, &y) == 4 && rows[i][2] != '\0' && rows[i][7] != '\0' && strlen(rows[i]) == 15u) {
      uint64_t v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}