#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* s) {
  int sign = 1;
  unsigned char c = (unsigned char)*s;
  if (c == '-') { sign = -1; s++; }
  else if (c == '+') { s++; }
  int v = 0;
  while (((c = (unsigned char)*s) - (unsigned char)'0') <= 9u) {
    v = v * 10 + (int)(c - (unsigned char)'0');
    s++;
  }
  return sign * v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    // Token starts
    const char* t0 = s;
    const char* p = s;
    while (*p && *p != '&') ++p;
    const char* t1 = *p ? (p + 1) : p;
    const char* q = t1;
    while (*q && *q != '&') ++q;
    const char* t2 = *q ? (q + 1) : q;

    int u = fast_atoi(t0 + 2);
    int sc = fast_atoi(t1 + 6);
    int f = fast_atoi(t2 + 5);

    uint64_t v = (uint64_t)((u & 2047) + sc + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
