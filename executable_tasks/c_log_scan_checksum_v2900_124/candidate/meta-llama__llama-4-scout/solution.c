#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    uint64_t v = 0;
    char p0 = rows[i][0];
    char p1 = rows[i][1];
    unsigned int x;
    unsigned int y;
    if (sscanf(rows[i], "%c%c%4u-%8X", &p0, &p1, &x, &y) == 4 && rows[i][13] == '\0' && rows[i][14] == '\0') {
      v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
    } else {
      continue;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}