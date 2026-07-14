#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    p++; // Skip 'r'
    int id = 0;
    while (*p >= '0' && *p <= '9') {
      id = id * 10 + (*p - '0');
      p++;
    }
    p++; // Skip comma
    int a = 0;
    while (*p >= '0' && *p <= '9') {
      a = a * 10 + (*p - '0');
      p++;
    }
    p++;
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;
    char flag[8];
    int j = 0;
    while (j < 7 && *p != '\0' && *p != '\n' && *p != '\r') {
      flag[j] = *p;
      j++;
      p++;
    }
    flag[j] = '\0';
    uint64_t v = (uint64_t)(b + c + 8);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}