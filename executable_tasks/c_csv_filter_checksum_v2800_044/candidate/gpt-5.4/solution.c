#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Format: r<id>,<a>,<b>,<c>,<flag> */
    if (*p == 'r') ++p;

    /* Skip id */
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == ',') ++p;

    /* Skip a (supports optional minus for robustness) */
    if (*p == '-') ++p;
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == ',') ++p;

    /* Parse b */
    int sign = 1;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (int)(*p - '0');
      ++p;
    }
    b *= sign;
    if (*p == ',') ++p;

    /* Parse c */
    sign = 1;
    if (*p == '-') {
      sign = -1;
      ++p;
    }
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (int)(*p - '0');
      ++p;
    }
    c *= sign;
    if (*p == ',') ++p;

    /* Check whether flag is exactly "hot" */
    uint64_t v = (uint64_t)(b + c + 28);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
