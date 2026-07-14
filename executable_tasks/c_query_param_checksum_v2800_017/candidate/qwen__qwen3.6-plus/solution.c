#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    const char* a1 = p;
    while (*a1 != '&') a1++;
    
    const char* a2 = a1 + 1;
    while (*a2 != '&') a2++;
    
    const char* pu = p + 2;
    int u = 0;
    while (*pu >= '0' && *pu <= '9') {
      u = u * 10 + (*pu - '0');
      pu++;
    }
    
    const char* ps = a1 + 7;
    int s = 0;
    while (*ps >= '0' && *ps <= '9') {
      s = s * 10 + (*ps - '0');
      ps++;
    }
    
    const char* pf = a2 + 6;
    int f = 0;
    while (*pf >= '0' && *pf <= '9') {
      f = f * 10 + (*pf - '0');
      pf++;
    }
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}