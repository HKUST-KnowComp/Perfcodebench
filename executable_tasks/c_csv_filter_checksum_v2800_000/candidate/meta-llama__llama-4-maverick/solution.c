#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    // Manually parse the row to avoid sscanf overhead
    int id = 0, a = 0, b = 0, c = 0;
    char flag[8];
    while (*row != ',') ++row;
    ++row; // Skip 'r' and move to the next field
    id = atoi(row);
    while (*row != ',') ++row;
    ++row;
    a = atoi(row);
    while (*row != ',') ++row;
    ++row;
    b = atoi(row);
    while (*row != ',') ++row;
    ++row;
    c = atoi(row);
    while (*row != ',') ++row;
    ++row;
    size_t flag_len = 0;
    while (flag_len < 7 && *row != '\0' && *row != '\n') {
      flag[flag_len++] = *row++;
    }
    flag[flag_len] = '\0';
    uint64_t v = (uint64_t)(b + c + 7);
    // Simplify the conditional logic by directly comparing the first few characters
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't') v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
