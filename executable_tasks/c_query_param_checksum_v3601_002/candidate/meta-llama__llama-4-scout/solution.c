#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    const char* p = strchr(row, '&');
    if (p == NULL) continue;
    int u = atoi(row + 2);
    p++;
    p = strchr(p, '&');
    if (p == NULL) continue;
    int s = atoi(p + 1);
    p++;
    int f = atoi(p + 1);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}