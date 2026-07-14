#include "interface.h"

#include <stdbool.h>
#include <stdint.h>

static inline int parse_int_at(const char* s, int start) {
  int v = 0;
  while (s[start] >= '0' && s[start] <= '9') {
    v = v * 10 + (s[start] - '0');
    ++start;
  }
  return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    int p1 = 0;
    while (row[p1] != ',') ++p1;
    int p2 = p1 + 1;
    while (row[p2] != ',') ++p2;
    int p3 = p2 + 1;
    while (row[p3] != ',') ++p3;
    int p4 = p3 + 1;
    while (row[p4] != ',') ++p4;
    int b = parse_int_at(row, p2 + 1);
    int c = parse_int_at(row, p3 + 1);
    bool hot = row[p4 + 1] == 'h';
    uint64_t v = (uint64_t)(b + c + 7);
    if (hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
