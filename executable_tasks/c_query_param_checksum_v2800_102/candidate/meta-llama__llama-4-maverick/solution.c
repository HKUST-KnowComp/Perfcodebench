#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // Skip "x="
    p += 2;
    int u = 0;
    while (*p != '&') {
      u = u * 10 + (*p - '0');
      p++;
    }
    // Skip "&sort="
    p += 6;
    int s = 0;
    while (*p != '&') {
      s = s * 10 + (*p - '0');
      p++;
    }
    // Skip "&from="
    p += 5;
    int f = 0;
    while (*p && *p != '&') {
      f = f * 10 + (*p - '0');
      p++;
    }
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
