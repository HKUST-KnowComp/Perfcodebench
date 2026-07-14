#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip 'r,'
    p += 2;
    
    // Skip id field
    while (*p != ',') p++;
    p++;
    
    // Skip a field
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
    
    // Read flag (up to 7 chars)
    char flag[8];
    int j = 0;
    while (j < 7 && *p != '\0') {
      flag[j++] = *p++;
    }
    flag[j] = '\0';
    
    uint64_t v = (uint64_t)(b + c + 29);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}