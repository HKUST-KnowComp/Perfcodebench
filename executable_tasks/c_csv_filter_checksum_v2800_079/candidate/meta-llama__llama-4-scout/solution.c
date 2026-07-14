#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    const char* comma1 = strchr(row, ',');
    if (comma1 == NULL) continue;
    const char* comma2 = strchr(comma1 + 1, ',');
    if (comma2 == NULL) continue;
    const char* comma3 = strchr(comma2 + 1, ',');
    if (comma3 == NULL) continue;
    const char* comma4 = strchr(comma3 + 1, ',');
    if (comma4 == NULL) continue;
    int id, a, b, c;
    sscanf(row, "r%d,%d,%d,%d," &id, &a, &b, &c);
    size_t flag_len = comma4 - comma3 - 1;
    const char* flag = comma3 + 1;
    uint64_t v = (uint64_t)(b + c + 17);
    if (flag_len == 3 && memcmp(flag, "hot", 3) == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}