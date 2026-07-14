#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Expected format: u=<int>&score=<int>&flag=<int> */
    p += 2; /* skip "u=" */
    int u = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      u = u * 10 + (*p - '0');
      ++p;
    }

    p += 7; /* skip "&score=" */
    int s = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      s = s * 10 + (*p - '0');
      ++p;
    }

    p += 6; /* skip "&flag=" */
    int f = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      f = f * 10 + (*p - '0');
      ++p;
    }

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
