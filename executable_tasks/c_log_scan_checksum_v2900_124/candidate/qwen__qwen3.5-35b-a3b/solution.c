#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (strlen(row) != 15) continue;
    if (row[6] != '-') continue;
    int valid = 1;
    for (int j = 2; j < 6; ++j) {
      if (row[j] < '0' || row[j] > '9') {
        valid = 0;
        break;
      }
    }
    if (!valid) continue;
    for (int j = 7; j < 15; ++j) {
      unsigned char c = row[j];
      if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
        valid = 0;
        break;
      }
    }
    if (!valid) continue;
    uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}