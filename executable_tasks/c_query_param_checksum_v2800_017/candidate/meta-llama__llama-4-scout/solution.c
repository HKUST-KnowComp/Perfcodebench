#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    const char* u_str = p + 2;
    const char* s_str = strstr(p, "&s=");
    if (s_str == NULL) continue;
    s_str += 3;
    const char* f_str = strstr(s_str, "&f=");
    if (f_str == NULL) continue;
    f_str += 3;
    int u = atoi(u_str);
    int s = atoi(s_str);
    int f = atoi(f_str);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}