#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    // Skip 'r' and id
    row = strchr(row, ',') + 1;
    // Skip a
    row = strchr(row, ',') + 1;
    // Parse b
    v += strtoul(row, (char**)&row, 10);
    // Skip ,
    ++row;
    // Parse c
    v += strtoul(row, (char**)&row, 10);
    v += 8;
    // Skip ,
    ++row;
    // Check flag
    if (strncmp(row, "hot", 3) == 0 && (row[3] == '\0' || row[3] == ',')) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}