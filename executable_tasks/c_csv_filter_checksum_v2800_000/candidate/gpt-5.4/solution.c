#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Skip leading 'r' and id field up to first comma. */
    if (*p == 'r') ++p;
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Skip a field up to second comma. */
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Parse b. */
    int neg = 0;
    uint64_t b = 0;
    if (*p == '-') {
      neg = 1;
      ++p;
    }
    while ((unsigned)(*p - '0') < 10U) {
      b = b * 10u + (uint64_t)(unsigned)(*p - '0');
      ++p;
    }
    int64_t sb = neg ? -(int64_t)b : (int64_t)b;
    if (*p == ',') ++p;

    /* Parse c. */
    neg = 0;
    uint64_t c = 0;
    if (*p == '-') {
      neg = 1;
      ++p;
    }
    while ((unsigned)(*p - '0') < 10U) {
      c = c * 10u + (uint64_t)(unsigned)(*p - '0');
      ++p;
    }
    int64_t sc = neg ? -(int64_t)c : (int64_t)c;
    if (*p == ',') ++p;

    /* Check whether flag is exactly "hot". */
    uint64_t v = (uint64_t)(sb + sc + 7);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
