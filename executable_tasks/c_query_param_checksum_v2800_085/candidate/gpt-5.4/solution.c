#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t parse_uint_after_prefix(const char *p, size_t skip) {
  const unsigned char *s = (const unsigned char *)p + skip;
  uint32_t v = 0;
  unsigned char c;
  while ((c = *s) >= '0' && c <= '9') {
    v = v * 10u + (uint32_t)(c - '0');
    ++s;
  }
  return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char *row = rows[i];

    uint32_t u = parse_uint_after_prefix(row, 2);      /* "u=" */
    const char *amp1 = strchr(row, '&');
    uint32_t s = parse_uint_after_prefix(amp1 + 1, 6); /* "score=" */
    const char *amp2 = strchr(amp1 + 1, '&');
    uint32_t f = parse_uint_after_prefix(amp2 + 1, 5); /* "flag=" */

    uint64_t v = (uint64_t)((u & 2047u) + s + f * 19u);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
