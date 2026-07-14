#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* p = (const unsigned char*)rows[i];

    /* Skip leading 'r' and the id field up to first comma. */
    if (*p == 'r') ++p;
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Skip field a up to second comma. */
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    /* Parse field b. */
    int neg = 0;
    int b = 0;
    if (*p == '-') {
      neg = 1;
      ++p;
    }
    while ((unsigned)(*p - '0') < 10U) {
      b = b * 10 + (int)(*p - '0');
      ++p;
    }
    if (neg) b = -b;
    if (*p == ',') ++p;

    /* Parse field c. */
    neg = 0;
    int c = 0;
    if (*p == '-') {
      neg = 1;
      ++p;
    }
    while ((unsigned)(*p - '0') < 10U) {
      c = c * 10 + (int)(*p - '0');
      ++p;
    }
    if (neg) c = -c;
    if (*p == ',') ++p;

    uint64_t v = (uint64_t)(b + c + 14);

    /* Check whether flag equals exactly "hot". */
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
