#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];

    // skip leading 'r'
    if (*p != 'r') continue;  // well-formed input, fallback
    p++;

    // skip id (integer)
    while (*p >= '0' && *p <= '9') p++;
    p++; // skip comma

    // skip a (integer)
    while (*p >= '0' && *p <= '9') p++;
    p++; // skip comma

    // parse b
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++; // skip comma

    // parse c
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++; // skip comma

    // parse flag (up to 7 characters)
    char flag[8];
    int flen = 0;
    while (*p != '\0' && flen < 7) {
      flag[flen++] = *p++;
    }
    flag[flen] = '\0';

    // compute v = b + c + 22, possibly add 29 if flag is "hot"
    uint64_t v = (uint64_t)(b + c + 22);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
