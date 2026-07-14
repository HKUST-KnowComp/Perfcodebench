#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    if (row[15] != '\0' || row[14] == '\0') continue;
    if (row[2] < '0' || row[2] > '9') continue;
    if (row[3] < '0' || row[3] > '9') continue;
    if (row[4] < '0' || row[4] > '9') continue;
    if (row[5] < '0' || row[5] > '9') continue;
    if (row[6] != '-') continue;
    int valid_hex = 1;
    for (int j = 7; j < 15; ++j) {
      char c = row[j];
      if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
        valid_hex = 0;
        break;
      }
    }
    if (!valid_hex) continue;
    uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}