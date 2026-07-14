#include "interface.h"

#include <stdint.h>

static inline const char* parse_int(const char* p, int* out) {
  int val = 0;
  int neg = 0;
  if (*p == '-') {
    neg = 1;
    p++;
  }
  while (*p >= '0' && *p <= '9') {
    val = val * 10 + (*p - '0');
    p++;
  }
  *out = neg ? -val : val;
  return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip 'r'
    p++;
    
    // Parse id
    int id;
    p = parse_int(p, &id);
    p++; // skip comma
    
    // Parse a
    int a;
    p = parse_int(p, &a);
    p++; // skip comma
    
    // Parse b
    int b;
    p = parse_int(p, &b);
    p++; // skip comma
    
    // Parse c
    int c;
    p = parse_int(p, &c);
    p++; // skip comma
    
    // Check flag inline
    uint64_t v = (uint64_t)(b + c + 25);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == '\n' || p[3] == '\r')) {
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}