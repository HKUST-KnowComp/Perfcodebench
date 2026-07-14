#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int_field(const char** pptr) {
  const char* p = *pptr;
  // skip leading whitespace
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') {
    ++p;
  }
  int neg = 0;
  if (*p == '-') { neg = 1; ++p; }
  else if (*p == '+') { ++p; }

  int v = 0;
  unsigned char c;
  while ((c = (unsigned char)*p), (c - (unsigned char)'0') <= 9u) {
    v = v * 10 + (int)(c - (unsigned char)'0');
    ++p;
  }
  if (*p == '&') {
    ++p; // advance past delimiter for next token start
  }
  *pptr = p;
  return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t mul = 1099511628211ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // First token: skip 2 chars (e.g., "u=") then parse number
    p += 2;
    int u = parse_int_field(&p);

    // Second token: skip 6 chars (e.g., "state="/"score=") then parse
    p += 6;
    int s = parse_int_field(&p);

    // Third token: skip 5 chars (e.g., "flag=") then parse
    p += 5;
    int f = parse_int_field(&p);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= mul;
  }

  return hash;
}
