#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Expect leading 'r' then id digits; move past 'r' if present
    if (*p == 'r') ++p;

    // Skip id and a fields (up to and past two commas)
    for (int skip = 0; skip < 2; ++skip) {
      while (*p && *p != ',') ++p;
      if (*p == ',') ++p;
    }

    // Parse b
    int b = 0;
    int sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    unsigned char ch;
    while ((ch = (unsigned char)*p) >= '0' && ch <= '9') {
      b = b * 10 + (ch - '0');
      ++p;
    }
    b *= sign;
    if (*p == ',') ++p;

    // Parse c
    int cval = 0;
    sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    while ((ch = (unsigned char)*p) >= '0' && ch <= '9') {
      cval = cval * 10 + (ch - '0');
      ++p;
    }
    cval *= sign;
    if (*p == ',') ++p;

    // Flag check: exact match to "hot"
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');

    uint64_t v = (uint64_t)(b + cval + 25);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
