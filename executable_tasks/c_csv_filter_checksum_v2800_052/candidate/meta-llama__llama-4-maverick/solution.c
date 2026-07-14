#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    // Skip "r" and parse id
    row += 1;
    int id = 0;
    while (*row != ',') id = id * 10 + (*row++ - '0');
    row += 1; // Skip ','
    // Parse a
    int a = 0;
    while (*row != ',') a = a * 10 + (*row++ - '0');
    row += 1; // Skip ','
    // Parse b
    int b = 0;
    while (*row != ',') b = b * 10 + (*row++ - '0');
    row += 1; // Skip ','
    // Parse c
    int c = 0;
    while (*row != ',') c = c * 10 + (*row++ - '0');
    row += 1; // Skip ','
    // Check flag
    uint64_t v = (uint64_t)(b + c + 13);
    if (strncmp(row, "hot", 3) == 0 && row[3] == '\0') v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
