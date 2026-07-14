#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t parse_uint_after_prefix(const char *p) {
  uint32_t v = 0;
  unsigned char c;
  while ((c = (unsigned char)*p) >= '0' && c <= '9') {
    v = v * 10u + (uint32_t)(c - '0');
    ++p;
  }
  return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];

    /* Expected format from baseline semantics:
       "u=<num>&sess=<num>&flag=<num>" */
    uint32_t u = parse_uint_after_prefix(p + 2);

    const char *amp1 = strchr(p, '&');
    const char *p1 = amp1 + 1;
    uint32_t s = parse_uint_after_prefix(p1 + 5);

    const char *amp2 = strchr(p1, '&');
    const char *p2 = amp2 + 1;
    uint32_t f = parse_uint_after_prefix(p2 + 5);

    uint64_t v = (uint64_t)((u & 2047u) + s + f * 19u);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
