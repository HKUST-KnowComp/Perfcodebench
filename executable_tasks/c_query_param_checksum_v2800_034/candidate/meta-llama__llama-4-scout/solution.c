#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    const char* u_start = p + 2;
    const char* s_start = strchr(p, '&') + 1 + 6;
    const char* f_start = strchr(s_start, '&') + 1 + 5;
    int u = atoi(u_start);
    int s = atoi(s_start);
    int f = atoi(f_start);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}