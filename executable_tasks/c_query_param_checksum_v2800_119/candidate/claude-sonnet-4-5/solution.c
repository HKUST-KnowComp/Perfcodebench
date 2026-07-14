#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline int parse_int(const char* str) {
  int result = 0;
  while (*str >= '0' && *str <= '9') {
    result = result * 10 + (*str - '0');
    ++str;
  }
  return result;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    
    // Parse u=XXX
    const char* p = row + 2;  // skip "u="
    int u = parse_int(p);
    
    // Find next '&' and parse status=XXX
    while (*p != '&') ++p;
    p += 8;  // skip "&status="
    int s = parse_int(p);
    
    // Find next '&' and parse flag=XXX
    while (*p != '&') ++p;
    p += 6;  // skip "&flag="
    int f = parse_int(p);
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}