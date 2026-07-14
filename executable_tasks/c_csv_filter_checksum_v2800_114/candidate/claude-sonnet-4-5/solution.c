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
    int b = parse_int(&p);
    p++; // skip comma
    
    // Parse c
    int c = parse_int(&p);
    p++; // skip comma
    
    // Check flag
    uint64_t v = (uint64_t)(b + c + 29);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}