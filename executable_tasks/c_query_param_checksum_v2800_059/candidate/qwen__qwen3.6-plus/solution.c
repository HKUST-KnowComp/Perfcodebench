#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    
    s += 2;
    int u = 0;
    while (*s >= '0' && *s <= '9') {
      u = u * 10 + (*s - '0');
      s++;
    }
    
    s += 7;
    int s_val = 0;
    while (*s >= '0' && *s <= '9') {
      s_val = s_val * 10 + (*s - '0');
      s++;
    }
    
    s += 6;
    int f = 0;
    while (*s >= '0' && *s <= '9') {
      f = f * 10 + (*s - '0');
      s++;
    }
    
    uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}