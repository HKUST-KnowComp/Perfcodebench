#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* s) {
  int sign = 1;
  if (*s == '-') { sign = -1; ++s; }
  int v = 0;
  unsigned char c;
  while ((c = (unsigned char)*s) - '0' <= 9) {
    v = v * 10 + (int)(c - '0');
    ++s;
  }
  return sign * v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Token 0: value starts at p + 2
    int u = fast_atoi(p + 2);

    // Find first '&'
    const char* q = p;
    while (*q != '&') { ++q; }

    // Token 1 starts at q + 1, value at +6 from token start
    const char* t1 = q + 1;
    int s = fast_atoi(t1 + 6);

    // Find second '&'
    const char* r = t1;
    while (*r != '&') { ++r; }

    // Token 2 starts at r + 1, value at +5 from token start
    const char* t2 = r + 1;
    int f = fast_atoi(t2 + 5);

    int sum = (u & 2047) + s + f * 19;
    uint64_t v = (uint64_t)sum;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
