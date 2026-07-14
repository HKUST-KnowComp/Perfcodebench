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
    int matched = sscanf(rows[i], "%c%c%4u-%8X", &p0, &p1, &x, &y);
    uint64_t v = 0;
    if (matched == 4 && strlen(rows[i]) == 15u) v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
