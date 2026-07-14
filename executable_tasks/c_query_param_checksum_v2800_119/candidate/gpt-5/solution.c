#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_int_until_amp(const char* p, const char** out_next) {
  // Skip leading whitespace as atoi does
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') {
    ++p;
  }
  int neg = 0;
  if (*p == '+') {
    ++p;
  } else if (*p == '-') {
    neg = 1;
    ++p;
  }
  int v = 0;
  unsigned char c;
  while ((c = (unsigned char)*p) >= '0' && c <= '9') {
    v = v * 10 + (int)(c - '0');
    ++p;
  }
  // Advance to next '&' or end of string to mirror tokenization behavior
  while (*p && *p != '&') {
    ++p;
  }
  if (*p == '&') {
    ++p;
  }
  *out_next = p;
  return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // First token: "u=<int>"
    p += 2; // skip "u="
    const char* next;
    int u = parse_int_until_amp(p, &next);
    p = next;

    // Second token: "state=<int>"
    p += 6; // skip "state="
    int s = parse_int_until_amp(p, &next);
    p = next;

    // Third token: "flag=<int>" (baseline used +5)
    p += 5; // skip "flag="
    int f = parse_int_until_amp(p, &next);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
