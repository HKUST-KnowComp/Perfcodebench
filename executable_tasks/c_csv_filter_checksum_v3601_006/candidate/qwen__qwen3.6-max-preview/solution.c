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
    
    // Parse b
    int b = 0;
    int sign_b = 1;
    if (*p == '-') { sign_b = -1; p++; }
    while (*p >= '0' && *p <= '9') b = b * 10 + (*p++ - '0');
    b *= sign_b;
    p++; // skip comma
    
    // Parse c
    int c = 0;
    int sign_c = 1;
    if (*p == '-') { sign_c = -1; p++; }
    while (*p >= '0' && *p <= '9') c = c * 10 + (*p++ - '0');
    c *= sign_c;
    p++; // skip comma
    
    // Check flag
    uint64_t v = (uint64_t)(b + c + 25);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') v += 29ULL;
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}