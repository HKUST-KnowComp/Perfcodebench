#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    /* Expected format: u=<int>&score=<int>&flag=<int> */
    p += 2; /* skip "u=" */
    unsigned u = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      u = u * 10u + (unsigned)(*p - '0');
      ++p;
    }

    p += 7; /* skip "&score=" */
    int s_sign = 1;
    if (*p == '-') {
      s_sign = -1;
      ++p;
    }
    unsigned s_abs = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      s_abs = s_abs * 10u + (unsigned)(*p - '0');
      ++p;
    }
    int s = (int)s_abs * s_sign;

    p += 6; /* skip "&flag=" */
    int f_sign = 1;
    if (*p == '-') {
      f_sign = -1;
      ++p;
    }
    unsigned f_abs = 0;
    while ((unsigned)(*p - '0') <= 9u) {
      f_abs = f_abs * 10u + (unsigned)(*p - '0');
      ++p;
    }
    int f = (int)f_abs * f_sign;

    uint64_t v = (uint64_t)((u & 2047u) + (unsigned long long)s + (unsigned long long)(f * 19));
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
