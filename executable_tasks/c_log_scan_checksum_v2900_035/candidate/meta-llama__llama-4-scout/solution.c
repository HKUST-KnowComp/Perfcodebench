#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (strlen(row) != 15) continue;
    char p0 = row[0];
    char p1 = row[1];
    unsigned int x;
    unsigned int y;
    if (sscanf(row, "%c%c%4u-%8X", &p0, &p1, &x, &y) != 4) continue;
    uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}