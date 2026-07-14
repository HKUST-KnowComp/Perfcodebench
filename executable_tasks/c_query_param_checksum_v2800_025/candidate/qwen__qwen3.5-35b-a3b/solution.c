#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    const char* p1 = strchr(p, '&');
    const char* p2 = strchr(p1 + 1, '&');
    
    // Parse u from first token (offset 2)
    const char* u_start = p + 2;
    int u = 0;
    while (*u_start >= '0' && *u_start <= '9') {
      u = u * 10 + (*u_start - '0');
      u_start++;
    }
    
    // Parse s from second token (offset 6)
    const char* s_start = p1 + 1 + 6;
    int s = 0;
    while (*s_start >= '0' && *s_start <= '9') {
      s = s * 10 + (*s_start - '0');
      s_start++;
    }
    
    // Parse f from third token (offset 5)
    const char* f_start = p2 + 1 + 5;
    int f = 0;
    while (*f_start >= '0' && *f_start <= '9') {
      f = f * 10 + (*f_start - '0');
      f_start++;
    }
    
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}