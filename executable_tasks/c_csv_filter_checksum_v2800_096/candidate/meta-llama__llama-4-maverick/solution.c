#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    // Skip "r" and the id
    row = strchr(row, ',') + 1;
    // Skip field 'a'
    row = strchr(row, ',') + 1;
    int b = 0;
    int sign = 1;
    if (*row == '-') {
      sign = -1;
      row++;
    }
    while (*row >= '0' && *row <= '9') {
      b = b * 10 + (*row - '0');
      row++;
    }
    b *= sign;
    // Skip ','
    row++;
    int c = 0;
    sign = 1;
    if (*row == '-') {
      sign = -1;
      row++;
    }
    while (*row >= '0' && *row <= '9') {
      c = c * 10 + (*row - '0');
      row++;
    }
    c *= sign;
    // Skip ','
    row++;
    uint64_t v = (uint64_t)(b + c + 11);
    if (strncmp(row, "hot", 3) == 0 && (row[3] == ',' || row[3] == '\0' || row[3] == '\n')) {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
