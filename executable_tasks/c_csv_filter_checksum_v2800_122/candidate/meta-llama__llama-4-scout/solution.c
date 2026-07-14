#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    int id, a, b, c;
    char flag[8];
    // Directly parse integers and the flag from the row
    sscanf(rows[i], "r%d,%d,%d,%d,%7s", &id, &a, &b, &c, flag);
    uint64_t v = (uint64_t)(b + c + 14);
    // Use a simple if statement instead of strcmp for efficiency
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}