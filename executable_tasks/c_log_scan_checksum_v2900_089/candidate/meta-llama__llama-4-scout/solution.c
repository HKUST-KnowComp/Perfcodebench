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
    unsigned int x = *(const unsigned int*)(row + 2);
    unsigned int y = *(const unsigned int*)(row + 7);
    if (p0 != 0 && p1 != 0 && x < 10000 && y < 4294967296ULL) {
      uint64_t v = (uint64_t)(row[6] + row[11]);
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}