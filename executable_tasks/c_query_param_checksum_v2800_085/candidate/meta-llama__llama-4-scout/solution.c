#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    const char* p0 = strstr(p, "u=");
    const char* p1 = strstr(p, "s=");
    const char* p2 = strstr(p, "f=");
    int u = atoi(p0 + 2);
    int s = atoi(p1 + 2);
    int f = atoi(p2 + 2);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}