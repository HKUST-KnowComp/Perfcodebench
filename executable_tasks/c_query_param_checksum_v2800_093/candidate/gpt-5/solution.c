#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline const char* skip_to_value(const char* p) {
  while (*p != '=') {
    ++p;
  }
  return p + 1; // position after '='
}

static inline int parse_number_advance(const char** pp) {
  const char* p = *pp;
  int neg = 0;
  if (*p == '-') { neg = 1; ++p; }
  else if (*p == '+') { ++p; }

  int v = 0;
  unsigned char c;
  while (((c = (unsigned char)*p) - '0') <= 9) {
    v = v * 10 + (c - '0');
    ++p;
  }
  // advance to next token start (skip to '&' or end, then skip '&')
  while (*p && *p != '&') ++p;
  if (*p == '&') ++p;

  *pp = p;
  return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    p = skip_to_value(p);
    int u = parse_number_advance(&p);

    p = skip_to_value(p);
    int s = parse_number_advance(&p);

    p = skip_to_value(p);
    int f = parse_number_advance(&p);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }

  return hash;
}
