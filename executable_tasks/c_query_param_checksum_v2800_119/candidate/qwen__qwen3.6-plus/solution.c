#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    s += 2;
    int u = 0;
    while (*s >= '0' && *s <= '9') {
      u = u * 10 + (*s - '0');
      s++;
    }

    s += 7;
    int sc = 0;
    while (*s >= '0' && *s <= '9') {
      sc = sc * 10 + (*s - '0');
      s++;
    }

    s += 6;
    int f = 0;
    while (*s >= '0' && *s <= '9') {
      f = f * 10 + (*s - '0');
      s++;
    }

    uint64_t v = (uint64_t)((u & 2047) + sc + f * 19);
    hash ^= (v + 1ULL);
    hash *= prime;
  }
  return hash;
}