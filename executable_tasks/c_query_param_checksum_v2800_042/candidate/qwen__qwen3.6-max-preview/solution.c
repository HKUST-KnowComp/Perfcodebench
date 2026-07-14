#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    p += 2;
    int u = 0;
    int neg = (*p == '-');
    if (neg) p++;
    else if (*p == '+') p++;
    while (*p >= '0' && *p <= '9') { u = u * 10 + (*p - '0'); p++; }
    if (neg) u = -u;
    
    p += 7;
    int s_val = 0;
    neg = (*p == '-');
    if (neg) p++;
    else if (*p == '+') p++;
    while (*p >= '0' && *p <= '9') { s_val = s_val * 10 + (*p - '0'); p++; }
    if (neg) s_val = -s_val;
    
    p += 6;
    int f = 0;
    neg = (*p == '-');
    if (neg) p++;
    else if (*p == '+') p++;
    while (*p >= '0' && *p <= '9') { f = f * 10 + (*p - '0'); p++; }
    if (neg) f = -f;
    
    uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}