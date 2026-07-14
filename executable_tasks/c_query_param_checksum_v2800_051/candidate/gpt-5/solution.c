#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_uint(const char* s, const char** end_out) {
  int v = 0;
  unsigned char c;
  while (((c = (unsigned char)*s) - (unsigned char)'0') <= 9u) {
    v = v * 10 + (int)(c - (unsigned char)'0');
    ++s;
  }
  if (end_out) *end_out = s;
  return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* cur = rows[i];

    // First field: starts with "u="
    cur += 2; // skip "u="
    int u = parse_uint(cur, &cur);

    // cur should now be at '&'
    ++cur; // skip '&'

    // Second field: starts with "score=" (6 chars)
    cur += 6; // skip "score="
    int s = parse_uint(cur, &cur);

    // cur should now be at '&'
    ++cur; // skip '&'

    // Third field: key of length 5 before '=' (baseline used p2 + 5)
    cur += 5;
    int f = parse_uint(cur, NULL);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
