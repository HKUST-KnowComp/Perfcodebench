#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static inline int parse_int(const char** p) {
  int val = 0;
  const char* s = *p;
  while (*s >= '0' && *s <= '9') {
    val = val * 10 + (*s - '0');
    s++;
  }
  *p = s;
  return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip 'r'
    p++;
    
    // Parse id (not used)
    while (*p >= '0' && *p <= '9') p++;
    p++; // skip comma
    
    // Parse a (not used)
    while (*p >= '0' && *p <= '9') p++;
    p++; // skip comma
    
    // Parse b
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++; // skip comma
    
    // Parse c
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++; // skip comma
    
    uint64_t v = (uint64_t)(b + c + 19);
    
    // Check if flag is "hot" (compare first 4 bytes including null terminator)
    if (*(uint32_t*)p == 0x746f68) { // "hot\0" in little-endian
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}