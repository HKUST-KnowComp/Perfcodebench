#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];

    // Skip leading 'r'
    p++;

    // Skip id field until comma
    while (*p != ',') p++;
    p++; // skip comma

    // Skip a field until comma
    while (*p != ',') p++;
    p++; // skip comma

    // Parse b
    int b = 0;
    int sign = 1;
    if (*p == '-') {
      sign = -1;
      p++;
    }
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    b *= sign;
    p++; // skip comma

    // Parse c
    int c = 0;
    sign = 1;
    if (*p == '-') {
      sign = -1;
      p++;
    }
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    c *= sign;
    p++; // skip comma (if any; if flag empty, p may point to '\0')

    // Flag starts at p
    uint64_t v = (uint64_t)(b + c + 25);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
