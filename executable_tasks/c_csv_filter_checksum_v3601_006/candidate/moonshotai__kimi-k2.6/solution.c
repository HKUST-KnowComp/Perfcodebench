#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    while (*p != ',') p++;
    p++;
    while (*p != ',') p++;
    p++;
    int b = 0;
    int sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    b *= sign;
    p++;
    int c = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    c *= sign;
    p++;
    uint64_t v = (uint64_t)(b + c + 25);
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') {
      p++;
    }
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      char d = p[3];
      if (d == '\0' || d == ' ' || d == '\t' || d == '\n' || d == '\r' || d == '\f' || d == '\v') {
        v += 29ULL;
      }
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
