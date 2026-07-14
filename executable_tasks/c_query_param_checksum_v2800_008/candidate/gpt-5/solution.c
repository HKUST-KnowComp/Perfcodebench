#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline const char* parse_int_fast(const char* p, int* out) {
  int neg = 0;
  if (*p == '-') { neg = 1; ++p; }
  else if (*p == '+') { ++p; }
  int v = 0;
  while ((unsigned)((unsigned char)*p - '0') <= 9u) {
    v = v * 10 + ((unsigned char)*p - '0');
    ++p;
  }
  *out = neg ? -v : v;
  return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    // Parse u after "u="
    const char* p = s + 2;
    int u;
    p = parse_int_fast(p, &u);
    while (*p && *p != '&') ++p;
    if (*p == '&') ++p;

    // Parse s after "short="
    const char* q = p + 6;
    int sval;
    q = parse_int_fast(q, &sval);
    while (*q && *q != '&') ++q;
    if (*q == '&') ++q;

    // Parse f after "flag="
    const char* r = q + 5;
    int f;
    (void)parse_int_fast(r, &f);

    uint64_t v = (uint64_t)((u & 2047) + sval + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
