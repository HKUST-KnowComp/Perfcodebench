#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip 'r' and parse id (unused)
    p++;
    while (*p != ',') p++;
    p++;
    
    // Skip a (unused)
    while (*p != ',') p++;
    p++;
    
    // Parse b
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;
    
    // Parse c
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;
    
    // Parse flag (up to 7 chars)
    char flag[8] = {0};
    int j = 0;
    while (*p != '\0' && j < 7) {
      flag[j++] = *p++;
    }
    
    uint64_t v = (uint64_t)(b + c + 16);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}