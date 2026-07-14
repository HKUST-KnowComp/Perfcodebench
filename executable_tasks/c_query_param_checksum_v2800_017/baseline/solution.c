#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    char buf[96];
    strcpy(buf, rows[i]);
    char* p0 = strtok(buf, "&");
    char* p1 = strtok(NULL, "&");
    char* p2 = strtok(NULL, "&");
    int u = atoi(p0 + 2);
    int s = atoi(p1 + 6);
    int f = atoi(p2 + 5);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
