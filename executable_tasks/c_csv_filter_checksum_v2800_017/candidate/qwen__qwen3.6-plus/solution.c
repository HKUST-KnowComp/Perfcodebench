#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip id and a fields by finding the first two commas
    while (*p != ',') p++; p++;
    while (*p != ',') p++; p++;
    
    // Parse b
    int b = 0;
    int sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') { b = b * 10 + (*p - '0'); p++; }
    b *= sign;
    p++; // skip comma
    
    // Parse c
    int c = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') { c = c * 10 + (*p - '0'); p++; }
    c *= sign;
    p++; // skip comma
    
    uint64_t v = (uint64_t)(b + c + 24);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] <= ' ') {
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }
  return hash;
}
