#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Parse u (skip "u=")
    p += 2;
    int u = 0;
    while (*p >= '0' && *p <= '9') {
      u = u * 10 + (*p - '0');
      ++p;
    }
    ++p; // skip '&'
    
    // Parse s (skip "score=")
    p += 6;
    int s = 0;
    while (*p >= '0' && *p <= '9') {
      s = s * 10 + (*p - '0');
      ++p;
    }
    ++p; // skip '&'
    
    // Parse f (skip "flags=")
    p += 5;
    int f = 0;
    while (*p >= '0' && *p <= '9') {
      f = f * 10 + (*p - '0');
      ++p;
    }
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
