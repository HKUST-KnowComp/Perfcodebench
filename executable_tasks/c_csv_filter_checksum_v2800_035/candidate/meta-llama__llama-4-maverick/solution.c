#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    // Skip 'r' and id
    while (*row != ',') ++row;
    ++row;
    // Skip a
    while (*row != ',') ++row;
    ++row;
    // Parse b
    v += (*row++ - '0') * 10;
    v += (*row++ - '0');
    row++;
    // Parse c
    v += (*row++ - '0') * 10;
    v += (*row++ - '0');
    v += 19;
    // Check flag
    row += 1; // Skip comma
    if (strncmp(row, "hot", 3) == 0 && (row[3] == ',' || row[3] == '\0')) {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
