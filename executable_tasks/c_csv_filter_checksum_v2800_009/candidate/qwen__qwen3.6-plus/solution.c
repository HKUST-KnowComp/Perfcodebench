#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    while (*p != ',') p++; p++;
    while (*p != ',') p++; p++;
    
    int b = 0;
    if (*p == '-') {
      p++;
      while (*p != ',') b = b * 10 - (*p++ - '0');
    } else {
      while (*p != ',') b = b * 10 + (*p++ - '0');
    }
    p++;
    
    int c = 0;
    if (*p == '-') {
      p++;
      while (*p != ',') c = c * 10 - (*p++ - '0');
    } else {
      while (*p != ',') c = c * 10 + (*p++ - '0');
    }
    p++;
    
    uint64_t v = (uint64_t)(b + c + 17);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] <= ' ') {
      v += 29ULL;
    }
    
    hash ^= v;
    hash *= FNV_PRIME;
  }
  return hash;
}
