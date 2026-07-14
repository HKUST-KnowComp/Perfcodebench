#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline const char* parse_int_fast(const char* p, int* out) {
  int neg = 0;
  char c = *p;
  if (c == '-') { neg = 1; ++p; }
  else if (c == '+') { ++p; }
  int v = 0;
  for (;;) {
    unsigned char d = (unsigned char)(*p - '0');
    if (d > 9) break;
    v = v * 10 + (int)d;
    ++p;
  }
  *out = neg ? -v : v;
  return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    if (*p == 'r') {
      ++p;
    }

    int tmp;
    p = parse_int_fast(p, &tmp);        // id
    if (*p == ',') ++p;

    p = parse_int_fast(p, &tmp);        // a
    if (*p == ',') ++p;

    int b;
    p = parse_int_fast(p, &b);          // b
    if (*p == ',') ++p;

    int c;
    p = parse_int_fast(p, &c);          // c
    if (*p == ',') ++p;

    int sum = b + c + 28;
    uint64_t v = (uint64_t)sum;

    int is_hot = 0;
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      char t = p[3];
      if (t == '\0' || t == ' ' || t == '\t' || t == '\n' || t == '\r' || t == '\f' || t == '\v') {
        is_hot = 1;
      }
    }

    if (is_hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
