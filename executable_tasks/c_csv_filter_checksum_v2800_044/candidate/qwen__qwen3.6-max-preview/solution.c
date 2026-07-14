#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip 'r' and id field
    p++;
    while (*p != ',') p++;
    p++;
    
    // Skip a field
    while (*p != ',') p++;
    p++;
    
    // Parse b field
    int b = 0;
    if (*p == '-') {
      p++;
      while (*p >= '0' && *p <= '9') b = b * 10 - (*p++ - '0');
    } else {
      while (*p >= '0' && *p <= '9') b = b * 10 + (*p++ - '0');
    }
    p++; // skip ','
    
    // Parse c field
    int c = 0;
    if (*p == '-') {
      p++;
      while (*p >= '0' && *p <= '9') c = c * 10 - (*p++ - '0');
    } else {
      while (*p >= '0' && *p <= '9') c = c * 10 + (*p++ - '0');
    }
    p++; // skip ','
    
    // Check flag field for exact match "hot"
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] <= ' ');
    
    uint64_t v = (uint64_t)(b + c + 28);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
