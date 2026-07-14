#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline int parse_int(const char* s) {
  int val = 0;
  while (*s >= '0' && *s <= '9') {
    val = val * 10 + (*s - '0');
    s++;
  }
  return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    
    // Parse u=XXX
    const char* p = row + 2;  // skip "u="
    int u = 0;
    while (*p >= '0' && *p <= '9') {
      u = u * 10 + (*p - '0');
      p++;
    }
    
    // Skip to status=XXX
    p += 8;  // skip "&status="
    int s = 0;
    while (*p >= '0' && *p <= '9') {
      s = s * 10 + (*p - '0');
      p++;
    }
    
    // Skip to flag=XXX
    p += 6;  // skip "&flag="
    int f = 0;
    while (*p >= '0' && *p <= '9') {
      f = f * 10 + (*p - '0');
      p++;
    }
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= prime;
  }
  
  return hash;
}
