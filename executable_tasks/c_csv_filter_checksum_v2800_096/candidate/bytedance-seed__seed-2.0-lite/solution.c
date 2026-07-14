#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];
    int commas = 0;
    // Skip first two CSV fields (id, a) to reach the b field
    while (*p && commas < 2) {
      if (*p == ',') commas++;
      p++;
    }
    // Parse integer value of b
    int b = 0;
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    // Parse integer value of c
    p++;
    int c = 0;
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    // Verify flag is exactly "hot" (matches strcmp behavior exactly)
    p++;
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');
    // Maintain original value and hash computation logic exactly
    uint64_t v = (uint64_t)(b + c + 11);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}