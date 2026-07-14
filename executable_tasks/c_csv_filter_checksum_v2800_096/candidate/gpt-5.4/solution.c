#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Skip leading 'r' and id field up to first comma */
    ++p;
    while (*p != ',') ++p;
    ++p;

    /* Skip a field up to next comma */
    while (*p != ',') ++p;
    ++p;

    /* Parse b */
    int sign = 1;
    int b = 0;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    while ((unsigned)(*p - '0') < 10u) {
      b = b * 10 + (*p - '0');
      ++p;
    }
    b *= sign;
    ++p; /* skip comma */

    /* Parse c */
    sign = 1;
    int c = 0;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    while ((unsigned)(*p - '0') < 10u) {
      c = c * 10 + (*p - '0');
      ++p;
    }
    c *= sign;
    ++p; /* skip comma */

    uint64_t v = (uint64_t)(b + c + 11);

    /* Check whether flag is exactly "hot" */
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
