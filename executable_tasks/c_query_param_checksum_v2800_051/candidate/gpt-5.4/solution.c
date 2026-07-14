#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* p = (const unsigned char*)rows[i];

    /* Parse u from "u=<digits>" */
    p += 2;
    unsigned int u = 0;
    unsigned char c = *p;
    while ((unsigned)(c - '0') <= 9u) {
      u = u * 10u + (unsigned int)(c - '0');
      c = *++p;
    }

    /* Skip "&sess=" */
    p += 6;
    unsigned int s = 0;
    c = *p;
    while ((unsigned)(c - '0') <= 9u) {
      s = s * 10u + (unsigned int)(c - '0');
      c = *++p;
    }

    /* Skip "&flag=" */
    p += 6;
    unsigned int f = 0;
    c = *p;
    while ((unsigned)(c - '0') <= 9u) {
      f = f * 10u + (unsigned int)(c - '0');
      c = *++p;
    }

    uint64_t v = (uint64_t)((u & 2047u) + s + f * 19u);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
