#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Skip leading 'r' and first integer field (id). */
    if (*p == 'r') ++p;
    while ((unsigned)(*p - '0') <= 9u) ++p;
    if (*p == ',') ++p;

    /* Skip second integer field (a), allowing optional sign. */
    if (*p == '-') ++p;
    while ((unsigned)(*p - '0') <= 9u) ++p;
    if (*p == ',') ++p;

    /* Parse third integer field (b). */
    int sign = 1;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    int b = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      b = b * 10 + (*p - '0');
      ++p;
    }
    b *= sign;
    if (*p == ',') ++p;

    /* Parse fourth integer field (c). */
    sign = 1;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    int c = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      c = c * 10 + (*p - '0');
      ++p;
    }
    c *= sign;
    if (*p == ',') ++p;

    /* Check whether flag is exactly "hot". */
    uint64_t v = (uint64_t)(b + c + 19);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
