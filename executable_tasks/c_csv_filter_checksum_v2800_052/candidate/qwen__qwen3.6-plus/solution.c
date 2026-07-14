#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i] + 1; // skip 'r'
    
    // skip id
    while (*p != ',') p++;
    p++;
    // skip a
    while (*p != ',') p++;
    p++;
    
    // parse b
    int b = 0;
    if (*p == '-') {
      p++;
      while (*p != ',') { b = b * 10 - (*p - '0'); p++; }
    } else {
      while (*p != ',') { b = b * 10 + (*p - '0'); p++; }
    }
    p++;
    
    // parse c
    int c = 0;
    if (*p == '-') {
      p++;
      while (*p != ',') { c = c * 10 - (*p - '0'); p++; }
    } else {
      while (*p != ',') { c = c * 10 + (*p - '0'); p++; }
    }
    p++;
    
    uint64_t v = (uint64_t)(b + c + 14);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && 
        (p[3] == '\0' || p[3] == '\r' || p[3] == '\n' || p[3] == ' ')) {
      v += 29ULL;
    }
    
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}