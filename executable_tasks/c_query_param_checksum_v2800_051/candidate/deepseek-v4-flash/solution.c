#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    // skip to first '=' and parse u (possibly signed)
    while (*s != '=') s++;
    s++;
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    int u = 0;
    while (*s >= '0' && *s <= '9') { u = u * 10 + (*s - '0'); s++; }
    u *= sign;
    // skip to second '=' and parse status
    while (*s != '=') s++;
    s++;
    sign = 1;
    if (*s == '-') { sign = -1; s++; }
    int s_val = 0;
    while (*s >= '0' && *s <= '9') { s_val = s_val * 10 + (*s - '0'); s++; }
    s_val *= sign;
    // skip to third '=' and parse flags
    while (*s != '=') s++;
    s++;
    sign = 1;
    if (*s == '-') { sign = -1; s++; }
    int f = 0;
    while (*s >= '0' && *s <= '9') { f = f * 10 + (*s - '0'); s++; }
    f *= sign;
    uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}