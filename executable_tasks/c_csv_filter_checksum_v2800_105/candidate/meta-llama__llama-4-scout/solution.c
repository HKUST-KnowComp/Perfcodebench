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
    // Use faster parsing with less overhead
    sscanf(rows[i], "r%d,%d,%d,%d,%7s", &id, &a, &b, &c, flag);
    uint64_t v = (uint64_t)(b + c + 20);
    // Use a simple and fast string comparison
    if (memcmp(flag, "hot", 4) == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}