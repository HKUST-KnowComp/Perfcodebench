#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    p += 2; // Skip "id"
    int u = 0;
    while (*p != '&') {
      u = u * 10 + (*p - '0');
      p++;
    }
    p += 6; // Skip "&state="
    int s = 0;
    while (*p != '&') {
      s = s * 10 + (*p - '0');
      p++;
    }
    p += 5; // Skip "&flag="
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
