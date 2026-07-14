#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const char* const* end = rows + row_count;
  while (rows < end) {
    const char* s = *rows++;
    int u = 0, sv = 0, f = 0;
    
    while (*s && (*s < '0' || *s > '9')) s++;
    while (*s >= '0' && *s <= '9') u = u * 10 + (*s++ - '0');
    
    while (*s && (*s < '0' || *s > '9')) s++;
    while (*s >= '0' && *s <= '9') sv = sv * 10 + (*s++ - '0');
    
    while (*s && (*s < '0' || *s > '9')) s++;
    while (*s >= '0' && *s <= '9') f = f * 10 + (*s++ - '0');
    
    uint64_t v = (uint64_t)((u & 2047) + sv + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
