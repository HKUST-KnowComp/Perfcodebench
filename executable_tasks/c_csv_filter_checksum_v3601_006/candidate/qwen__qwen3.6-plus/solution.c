#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Skip 'r' and id
    while (*p != ',') p++;
    p++;
    // Skip a
    while (*p != ',') p++;
    p++;
    
    // Parse b
    int b = 0;
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;
    
    // Parse c
    int c = 0;
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;
    
    uint64_t v = (uint64_t)(b + c + 25);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }
  return hash;
}
