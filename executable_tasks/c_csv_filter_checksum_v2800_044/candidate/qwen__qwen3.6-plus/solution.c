#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    p++; // skip 'r'
    while (*p != ',') p++; p++; // skip id
    while (*p != ',') p++; p++; // skip a
    
    int b = 0;
    int neg = (*p == '-');
    if (neg) p++;
    while (*p != ',') { b = b * 10 + (*p - '0'); p++; }
    if (neg) b = -b;
    p++;
    
    int c = 0;
    neg = (*p == '-');
    if (neg) p++;
    while (*p != ',') { c = c * 10 + (*p - '0'); p++; }
    if (neg) c = -c;
    p++;
    
    uint64_t v = (uint64_t)(b + c + 28);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] <= ' ')) {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}