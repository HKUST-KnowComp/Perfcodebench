#include "interface.h"

#include <stdint.h>
#include <string.h>

static inline int parse_int_from(const char* p) {
  int v = 0;
  unsigned char c;
  while ((unsigned)( (c = (unsigned char)*p) - '0') <= 9u) {
    v = v * 10 + (c - '0');
    ++p;
  }
  return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    const char* amp1 = strchr(p, '&');
    const char* t1 = amp1 + 1; // start of second token
    const char* amp2 = strchr(t1, '&');
    const char* t2 = amp2 + 1; // start of third token

    int u = parse_int_from(p + 2);
    int s = parse_int_from(t1 + 6);
    int f = parse_int_from(t2 + 5);

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
