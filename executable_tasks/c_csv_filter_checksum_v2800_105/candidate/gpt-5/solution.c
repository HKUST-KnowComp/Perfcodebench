#include "interface.h"

#include <stdint.h>

static inline const char* parse_int_fast(const char* p, int* out) {
  int sign = 1;
  if (*p == '-') {
    sign = -1;
    ++p;
  }
  unsigned int u = 0;
  unsigned char ch = (unsigned char)*p;
  while (ch >= '0' && ch <= '9') {
    u = u * 10u + (unsigned)(ch - '0');
    ch = (unsigned char)*++p;
  }
  *out = sign < 0 ? -(int)u : (int)u;
  return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1 64-bit offset basis
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Expect 'r' prefix; advance if present
    if (*p == 'r') ++p;

    int tmp;

    // id
    p = parse_int_fast(p, &tmp);
    if (*p == ',') ++p;

    // a
    p = parse_int_fast(p, &tmp);
    if (*p == ',') ++p;

    // b
    int b;
    p = parse_int_fast(p, &b);
    if (*p == ',') ++p;

    // c
    int c;
    p = parse_int_fast(p, &c);
    if (*p == ',') ++p;

    // flag token starts at p; we only need to know if it is exactly "hot"
    uint64_t v = (uint64_t)((b + c + 20));
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
      v += 29ULL;
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL; // FNV-1 prime
  }
  return hash;
}
