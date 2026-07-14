#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    const char* p = s;
    while (*p && *p != '&') p++;

    int u = 0;
    const char* d = s + 2;
    if (*d == '-') { d++; while (*d >= '0' && *d <= '9') u = u * 10 - (*d++ - '0'); }
    else { while (*d >= '0' && *d <= '9') u = u * 10 + (*d++ - '0'); }

    const char* p1 = p + 1;
    p = p1;
    while (*p && *p != '&') p++;

    int sv = 0;
    d = p1 + 6;
    if (*d == '-') { d++; while (*d >= '0' && *d <= '9') sv = sv * 10 - (*d++ - '0'); }
    else { while (*d >= '0' && *d <= '9') sv = sv * 10 + (*d++ - '0'); }

    int f = 0;
    d = p + 6;
    if (*d == '-') { d++; while (*d >= '0' && *d <= '9') f = f * 10 - (*d++ - '0'); }
    else { while (*d >= '0' && *d <= '9') f = f * 10 + (*d++ - '0'); }

    uint64_t v = (uint64_t)((u & 2047) + sv + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}