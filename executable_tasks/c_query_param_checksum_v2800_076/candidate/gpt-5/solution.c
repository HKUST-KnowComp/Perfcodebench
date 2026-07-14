#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline int parse_int_until(const char* s, const char** out_next, char delim) {
  const char* p = s;
  int neg = 0;
  char c = *p;
  if (c == '-') { neg = 1; p++; }
  else if (c == '+') { p++; }
  unsigned int uv = 0;
  while (1) {
    c = *p;
    if (c == delim || c == '\0') break;
    uv = uv * 10u + (unsigned)(c - '0');
    p++;
  }
  *out_next = p; // points at delim or end
  int val = (int)uv;
  return neg ? -val : val;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    const char* next;

    // u=<int>
    int u = parse_int_until(p + 2, &next, '&');
    p = next;
    if (*p == '&') ++p;

    // stage=<int>
    int s = parse_int_until(p + 6, &next, '&');
    p = next;
    if (*p == '&') ++p;

    // flag=<int>
    int f = parse_int_until(p + 5, &next, '&');

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
