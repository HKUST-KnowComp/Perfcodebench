#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (row[2] == '-' || row[6] != '-') continue;
    char p0 = row[0];
    char p1 = row[1];
    unsigned int x = (row[3] - '0') * 1000 + (row[4] - '0') * 100 + (row[5] - '0') * 10 + (row[6] - '0');
    unsigned int y = 0;
    for (int j = 7; j < 15; ++j) {
      char c = row[j];
      if (c >= '0' && c <= '9') y = y * 16 + (c - '0');
      else if (c >= 'A' && c <= 'F') y = y * 16 + (c - 'A' + 10);
      else break;
    }
    uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}