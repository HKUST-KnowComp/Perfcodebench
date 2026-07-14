#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    const char* p0 = strchr(row, 'u') + 2;
    const char* p1 = strchr(p0, 's') + 6;
    const char* p2 = strchr(p1, 'f') + 5;
    int u = atoi(p0);
    int s = atoi(p1);
    int f = atoi(p2);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}