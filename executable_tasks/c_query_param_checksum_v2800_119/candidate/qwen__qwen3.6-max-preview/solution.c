#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Parse u
    while (*p != '=') p++;
    p++;
    int u = 0;
    int sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while ((unsigned char)(*p - '0') <= 9) { u = u * 10 + (*p - '0'); p++; }
    u *= sign;

    // Parse s
    while (*p != '=') p++;
    p++;
    int s = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while ((unsigned char)(*p - '0') <= 9) { s = s * 10 + (*p - '0'); p++; }
    s *= sign;

    // Parse f
    while (*p != '=') p++;
    p++;
    int f = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while ((unsigned char)(*p - '0') <= 9) { f = f * 10 + (*p - '0'); p++; }
    f *= sign;

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}