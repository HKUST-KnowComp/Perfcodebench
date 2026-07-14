#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    ++p; /* skip 'r' */

    /* skip first integer (id) */
    if (*p == '-') ++p;
    while (*p >= '0' && *p <= '9') ++p;
    ++p; /* skip comma */

    /* skip second integer (a) */
    if (*p == '-') ++p;
    while (*p >= '0' && *p <= '9') ++p;
    ++p; /* skip comma */

    /* parse third integer (b) */
    int sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      ++p;
    }
    b *= sign;
    ++p; /* skip comma */

    /* parse fourth integer (c) */
    sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      ++p;
    }
    ++p; /* skip comma */

    /* evaluate flag string */
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');

    uint64_t v = (uint64_t)(b + c + 29);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
