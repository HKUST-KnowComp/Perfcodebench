#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_MULT = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    s++; // Skip leading 'r' character
    // Scan past the first two commas to skip unused id and a fields
    int commas = 0;
    while (*s && commas < 2) {
      if (*s == ',') commas++;
      s++;
    }
    // Parse b value
    int b = 0;
    while (*s >= '0' && *s <= '9') {
      b = b * 10 + (*s - '0');
      s++;
    }
    s++; // Skip comma before c
    // Parse c value
    int c = 0;
    while (*s >= '0' && *s <= '9') {
      c = c * 10 + (*s - '0');
      s++;
    }
    s++; // Skip comma before flag
    // Check for "hot" flag with fast direct byte comparisons
    const int is_hot = (s[0] == 'h' && s[1] == 'o' && s[2] == 't');
    // Exact original value calculation and hash update to preserve correctness
    uint64_t v = (uint64_t)(b + c + 7);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= FNV_MULT;
  }
  return hash;
}