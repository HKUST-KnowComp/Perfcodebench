#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  int id, a, b, c;
  char flag[8];
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    p++; // Skip 'r'

    // Parse id
    id = 0;
    while (*p >= '0' && *p <= '9') {
      id = id * 10 + (*p++ - '0');
    }
    p++; // Skip comma

    // Parse a
    a = 0;
    while (*p >= '0' && *p <= '9') {
      a = a * 10 + (*p++ - '0');
    }
    p++; // Skip comma

    // Parse b
    b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p++ - '0');
    }
    p++; // Skip comma

    // Parse c
    c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p++ - '0');
    }
    p++; // Skip comma

    // Parse flag
    int i_flag = 0;
    while (i_flag < 7 && *p != ',' && *p != '\0') {
      flag[i_flag++] = *p++;
    }
    flag[i_flag] = '\0';

    uint64_t v = (uint64_t)(b + c + 17);
    if (strcmp(flag, "hot") == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}