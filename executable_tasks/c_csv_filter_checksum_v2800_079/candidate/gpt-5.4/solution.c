#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Skip leading 'r' and parse id field until first comma. */
    if (*p == 'r') ++p;
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Parse a, skip to next comma. */
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Parse b. */
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
    if (*p == ',') ++p;

    /* Parse c. */
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
    if (*p == ',') ++p;

    /* Parse flag: baseline uses %7s, so only first token up to whitespace, max 7 chars.
       In this dataset the relevant comparison is against "hot". */
    uint64_t v = (uint64_t)(b + c + 17);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      char t = p[3];
      if (t == '\0' || t == ' ' || t == '\t' || t == '\n' || t == '\r' || t == '\f' || t == '\v') {
        v += 29ULL;
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
