#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    if (*s != 'r') continue;
    s++;

    int id = 0, a = 0, b = 0, c = 0;
    int sign;

    // Parse id
    sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { id = id * 10 + (*s++ - '0'); }
    id *= sign;
    if (*s == ',') s++;

    // Parse a
    sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { a = a * 10 + (*s++ - '0'); }
    a *= sign;
    if (*s == ',') s++;

    // Parse b
    sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { b = b * 10 + (*s++ - '0'); }
    b *= sign;
    if (*s == ',') s++;

    // Parse c
    sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { c = c * 10 + (*s++ - '0'); }
    c *= sign;
    if (*s == ',') s++;

    // Parse flag
    char flag[8];
    int len = 0;
    while (len < 7 && *s != '\0') flag[len++] = *s++;
    flag[len] = '\0';

    uint64_t v = (uint64_t)(b + c + 14);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0')
      v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}