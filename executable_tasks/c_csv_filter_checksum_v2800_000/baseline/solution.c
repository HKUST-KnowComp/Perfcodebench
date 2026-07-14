#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    int id = 0;
    int a = 0;
    int b = 0;
    int c = 0;
    char flag[8];
    flag[0] = '\0';
    sscanf(rows[i], "r%d,%d,%d,%d,%7s", &id, &a, &b, &c, flag);
    uint64_t v = (uint64_t)(b + c + 7);
    if (strcmp(flag, "hot") == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
