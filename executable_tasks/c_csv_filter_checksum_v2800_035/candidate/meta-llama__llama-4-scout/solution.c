#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    int id, a, b, c;
    char flag[8];
    // Use faster parsing with strtol and strncmp for flag
    char* endptr;
    id = (int)strtol(rows[i], &endptr, 10);
    endptr++; // skip comma
    a = (int)strtol(endptr, &endptr, 10);
    endptr++; // skip comma
    b = (int)strtol(endptr, &endptr, 10);
    endptr++; // skip comma
    c = (int)strtol(endptr, &endptr, 10);
    endptr++; // skip comma
    strncpy(flag, endptr, 7);
    flag[7] = '\0';

    uint64_t v = (uint64_t)(b + c + 19);
    // Use strncmp for efficient string comparison
    if (strncmp(flag, "hot", 3) == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}