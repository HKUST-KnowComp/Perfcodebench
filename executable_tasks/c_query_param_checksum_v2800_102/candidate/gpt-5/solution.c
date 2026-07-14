#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int(const char* p, const char** out_end) {
  int sign = 1;
  char c = *p;
  if (c == '-') { sign = -1; c = *++p; }
  else if (c == '+') { c = *++p; }
  int v = 0;
  while ((unsigned)(c - '0') <= 9u) {
    v = v * 10 + (c - '0');
    c = *++p;
  }
  *out_end = p;
  return sign * v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // First token: "u=" then number until '&'
    p += 2; // skip "u="
    int u = parse_int(p, &p);
    if (*p == '&') ++p;

    // Second token: "score=" then number until '&'
    p += 6; // skip "score="
    int s = parse_int(p, &p);
    if (*p == '&') ++p;

    // Third token: "flag=" then number until end or '&'
    p += 5; // skip "flag="
    int f = parse_int(p, &p);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
