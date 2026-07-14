#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];
    int commas = 0;
    // Skip past first two commas to reach the start of b (unused id and a are skipped entirely)
    while (*p && commas < 2) {
      if (*p == ',') commas++;
      p++;
    }
    // Parse b from current position
    int b = 0;
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    // Move to parse c
    p++;
    int c = 0;
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    // Move to flag and check for "hot" with direct byte comparisons (no strcmp overhead)
    p++;
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't');
    // Original hash logic preserved exactly to maintain correct checksum
    uint64_t v = (uint64_t)(b + c + 28);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}