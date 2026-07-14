#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Skip leading 'r' and id until first comma */
    if (*p == 'r') ++p;
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Skip field a until next comma */
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Parse field b */
    int sign = 1;
    int b = 0;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    while ((unsigned)(*p - '0') <= 9u) {
      b = b * 10 + (*p - '0');
      ++p;
    }
    b *= sign;
    if (*p == ',') ++p;

    /* Parse field c */
    sign = 1;
    int c = 0;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    while ((unsigned)(*p - '0') <= 9u) {
      c = c * 10 + (*p - '0');
      ++p;
    }
    c *= sign;
    if (*p == ',') ++p;

    /* Final field is flag; baseline only checks for exact "hot" */
    uint64_t v = (uint64_t)(b + c + 29);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
