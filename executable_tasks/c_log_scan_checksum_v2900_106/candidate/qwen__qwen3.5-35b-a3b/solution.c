#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    if (rows[i][15] != '\0' || rows[i][14] == '\0') continue;

    if (rows[i][2] < '0' || rows[i][2] > '9') continue;
    if (rows[i][3] < '0' || rows[i][3] > '9') continue;
    if (rows[i][4] < '0' || rows[i][4] > '9') continue;
    if (rows[i][5] < '0' || rows[i][5] > '9') continue;

    if (rows[i][6] != '-') continue;

    int valid_hex = 1;
    for (int j = 7; j < 15; ++j) {
        unsigned char c = rows[i][j];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            valid_hex = 0;
            break;
        }
    }
    if (!valid_hex) continue;

    uint64_t v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}