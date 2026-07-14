#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* ptr = rows[i] + 1; // Skip 'r'

    // Skip first integer (id)
    while (*ptr != ',') ptr++;
    ptr++; // Skip comma

    // Skip second integer (a)
    while (*ptr != ',') ptr++;
    ptr++; // Skip comma

    // Parse b
    int b = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      b = b * 10 + (*ptr - '0');
      ptr++;
    }
    ptr++; // Skip comma

    // Parse c
    int c = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      c = c * 10 + (*ptr - '0');
      ptr++;
    }

    uint64_t v = (uint64_t)(b + c + 25);
    if (ptr[0] == 'h' && ptr[1] == 'o' && ptr[2] == 't' && ptr[3] == '\0') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}