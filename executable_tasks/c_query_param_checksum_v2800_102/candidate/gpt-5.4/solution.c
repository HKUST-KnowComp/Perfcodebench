#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* p = (const unsigned char*)rows[i];

    /* Format is structured like: u=<num>&sess=<num>&flag=<num> */
    p += 2; /* skip "u=" */
    unsigned int u = 0;
    while (*p >= '0' && *p <= '9') {
      u = u * 10u + (unsigned int)(*p - '0');
      ++p;
    }

    /* skip "&sess=" */
    p += 6;
    unsigned int s = 0;
    while (*p >= '0' && *p <= '9') {
      s = s * 10u + (unsigned int)(*p - '0');
      ++p;
    }

    /* skip "&flag=" */
    p += 6;
    unsigned int f = 0;
    while (*p >= '0' && *p <= '9') {
      f = f * 10u + (unsigned int)(*p - '0');
      ++p;
    }

    uint64_t v = (uint64_t)((u & 2047u) + s + f * 19u);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
