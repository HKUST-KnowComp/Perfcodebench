#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_uint(const char** p, size_t skip) {
  const char* s = *p;
  s += skip;
  uint64_t v = 0;
  while ((unsigned char)(*s - '0') < 10) {
    v = v * 10 + (uint64_t)(*s - '0');
    ++s;
  }
  *p = s;
  return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    uint64_t u = parse_uint(&s, 3);  // skip "u="
    uint64_t s_val = parse_uint(&s, 7);  // skip "&s="
    uint64_t f = parse_uint(&s, 7);  // skip "&f="

    uint64_t v = (u & 2047) + s_val + f * 19;
    
    hash ^= (v + 1);
    hash *= prime;
  }

  return hash;
}