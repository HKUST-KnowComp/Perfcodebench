#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Parse u
    while (*p != '=') p++;
    p++;
    int u = 0;
    if (*p == '-') { p++; while (*p >= '0' && *p <= '9') { u = u * 10 - (*p - '0'); p++; } }
    else { while (*p >= '0' && *p <= '9') { u = u * 10 + (*p - '0'); p++; } }
    
    // Parse s
    while (*p != '=') p++;
    p++;
    int s = 0;
    if (*p == '-') { p++; while (*p >= '0' && *p <= '9') { s = s * 10 - (*p - '0'); p++; } }
    else { while (*p >= '0' && *p <= '9') { s = s * 10 + (*p - '0'); p++; } }
    
    // Parse f
    while (*p != '=') p++;
    p++;
    int f = 0;
    if (*p == '-') { p++; while (*p >= '0' && *p <= '9') { f = f * 10 - (*p - '0'); p++; } }
    else { while (*p >= '0' && *p <= '9') { f = f * 10 + (*p - '0'); p++; } }
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}