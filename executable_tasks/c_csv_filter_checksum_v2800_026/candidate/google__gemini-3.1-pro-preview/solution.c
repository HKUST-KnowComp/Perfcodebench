#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip 'r' and id
    while (*p != ',') p++;
    p++;
    
    // Skip a
    while (*p != ',') p++;
    p++;
    
    // Parse b
    while (*p == ' ' || *p == '\t') p++;
    int b = 0;
    int sign_b = 1;
    if (*p == '-') { sign_b = -1; p++; }
    while (*p >= '0' && *p <= '9') {
        b = b * 10 + (*p - '0');
        p++;
    }
    b *= sign_b;
    while (*p != ',') p++;
    p++;
    
    // Parse c
    while (*p == ' ' || *p == '\t') p++;
    int c = 0;
    int sign_c = 1;
    if (*p == '-') { sign_c = -1; p++; }
    while (*p >= '0' && *p <= '9') {
        c = c * 10 + (*p - '0');
        p++;
    }
    c *= sign_c;
    while (*p != ',') p++;
    p++;
    
    // Check flag
    while (*p == ' ' || *p == '\t') p++;
    uint64_t v = (uint64_t)(b + c + 10);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] <= ' ')) {
        v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
