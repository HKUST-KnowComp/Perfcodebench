#include "interface.h"

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
    int sp = 0;
    while (!(row[sp] == '&' && row[sp + 1] == 's')) ++sp;
    sp += 7;
    int fp = 0;
    while (!(row[fp] == '&' && row[fp + 1] == 'f')) ++fp;
    fp += 6;
    int u = parse_int_at(row, 2);
    int s = parse_int_at(row, sp);
    int f = parse_int_at(row, fp);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
