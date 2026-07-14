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
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    
    // Parse u=<value>
    const char* p = row + 2;
    int u = parse_int(p);
    
    // Skip to status=<value>
    while (*p != '&') p++;
    p += 8; // skip "&status="
    int s = parse_int(p);
    
    // Skip to flag=<value>
    while (*p != '&') p++;
    p += 6; // skip "&flag="
    int f = parse_int(p);
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}