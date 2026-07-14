#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    /* skip first field: r + id */
    while (*p != ',' && *p) ++p;
    ++p; /* skip comma */
    /* skip second field: a */
    while (*p != ',' && *p) ++p;
    ++p;
    /* parse third field: b */
    uint64_t b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (uint64_t)(*p - '0');
      ++p;
    }
    ++p; /* skip comma */
    /* parse fourth field: c */
    uint64_t c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (uint64_t)(*p - '0');
      ++p;
    }
    ++p; /* skip final comma */
    /* compute base value */
    uint64_t v = b + c + 8;
    /* check if flag field is exactly 'hot' */
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29;
    }
    /* hash update */
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
