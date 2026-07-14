#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for(size_t i = 0; i < row_count; ++i) {
    const char *s = rows[i];
    /* field1: "u="  */
    uint64_t u = 0;
    s += 2; /* skip "u=" */
    while(*s>='0' && *s<='9'){ u = u*10U + (*s++ -'0'); }
    ++s; /* skip '&' */
    /* field2: "scope=" */
    uint64_t sc = 0;
    s += 6; /* skip "scope=" */
    while(*s>='0' && *s<='9'){ sc = sc*10U + (*s++ -'0'); }
    ++s; /* skip '&' */
    /* field3: "flag=" */
    uint64_t f = 0;
    s += 5; /* skip "flag=" */
    while(*s>='0' && *s<='9'){ f = f*10U + (*s++ -'0'); }

    uint64_t v  = (u & 2047ULL) + sc + f*19ULL;
    uint64_t v1 = v + 1ULL;
    hash ^= v1;
    hash *= 1099511628211ULL;
  }
  return hash;
}