#include "interface.h"
#include <stdint.h>

static inline int fast_atoi_advance(const char** pp) {
  const char* p = *pp;
  // Optional leading whitespace handling (matches atoi behavior for safety)
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') {
    ++p;
  }
  int sign = 1;
  if (*p == '-') { sign = -1; ++p; }
  else if (*p == '+') { ++p; }
  int v = 0;
  for (;;) {
    unsigned char uc = (unsigned char)*p;
    unsigned d = (unsigned)(uc - (unsigned char)'0');
    if (d > 9u) break;
    v = v * 10 + (int)d;
    ++p;
  }
  *pp = p;
  return sign * v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // First token: assume key length 2 (e.g., "u=")
    p += 2;                   // move to value start
    int u = fast_atoi_advance(&p);
    if (*p == '&') ++p;      // skip delimiter

    // Second token: assume key length 6
    p += 6;                   // move to value start
    int s = fast_atoi_advance(&p);
    if (*p == '&') ++p;      // skip delimiter

    // Third token: assume key length 5
    p += 5;                   // move to value start
    int f = fast_atoi_advance(&p);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
