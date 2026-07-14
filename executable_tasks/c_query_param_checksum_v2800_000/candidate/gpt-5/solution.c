#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char *p) {
  int neg = 0;
  if (*p == '-') { neg = 1; ++p; }
  else if (*p == '+') { ++p; }
  int v = 0;
  unsigned char uc;
  while (((uc = (unsigned char)*p) - (unsigned char)'0') <= 9u) {
    v = v * 10 + (int)(uc - (unsigned char)'0');
    ++p;
  }
  return neg ? -v : v;
}

static inline const char* find_amp(const char* s) {
  while (*s != '&') ++s;
  return s;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];

    int u = fast_atoi(p + 2);

    const char *q = find_amp(p) + 1; // start of second token
    int s = fast_atoi(q + 6);

    const char *r = find_amp(q) + 1; // start of third token
    int f = fast_atoi(r + 5);

    int tmp = (u & 2047) + s + f * 19;
    uint64_t v = (uint64_t)tmp;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
