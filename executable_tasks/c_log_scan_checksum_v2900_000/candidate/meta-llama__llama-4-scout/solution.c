#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    if (strlen(p) != 15) continue;
    char p0 = p[0];
    char p1 = p[1];
    unsigned int x = (p[2] - '0') * 1000 + (p[3] - '0') * 100 + (p[4] - '0') * 10 + (p[5] - '0');
    unsigned int y = 0;
    for (int j = 6; j < 14; ++j) {
      char c = p[j];
      y = y * 16 + (c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - 'A' + 10 : c >= 'a' && c <= 'f' ? c - 'a' + 10 : 0);
    }
    uint64_t v = (uint64_t)((unsigned char)p[2] + (unsigned char)p[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}