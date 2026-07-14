#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    p += 2; // Skip 'r,'

    // Parse id
    int id = 0, sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') { id = id * 10 + (*p++ - '0'); }
    id *= sign; p++;

    // Parse a
    int a = 0;
    if (*p == '-') { sign = -1; p++; } else sign = 1;
    while (*p >= '0' && *p <= '9') { a = a * 10 + (*p++ - '0'); }
    a *= sign; p++;

    // Parse b
    int b = 0;
    if (*p == '-') { sign = -1; p++; } else sign = 1;
    while (*p >= '0' && *p <= '9') { b = b * 10 + (*p++ - '0'); }
    b *= sign; p++;

    // Parse c
    int c = 0;
    if (*p == '-') { sign = -1; p++; } else sign = 1;
    while (*p >= '0' && *p <= '9') { c = c * 10 + (*p++ - '0'); }
    c *= sign; p++;

    // Parse flag
    char flag[8];
    int j = 0;
    while (j < 7 && *p != '\n' && *p != '\0') {
      flag[j] = *p++;
      j++;
    }
    flag[j] = '\0';

    uint64_t v = (uint64_t)(b + c + 24);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}