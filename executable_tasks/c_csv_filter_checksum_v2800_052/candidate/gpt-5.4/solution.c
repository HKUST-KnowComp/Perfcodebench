#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Skip leading 'r' and parse id until first comma. */
    if (*p == 'r') ++p;
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Parse a until comma, then skip it. */
    while ((unsigned)(*p - '0') <= 9u) ++p;
    if (*p == ',') ++p;

    /* Parse b. */
    int b = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      b = b * 10 + (*p - '0');
      ++p;
    }
    if (*p == ',') ++p;

    /* Parse c. */
    int c = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      c = c * 10 + (*p - '0');
      ++p;
    }
    if (*p == ',') ++p;

    uint64_t v = (uint64_t)(b + c + 13);

    /* Check whether flag is exactly "hot". */
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
