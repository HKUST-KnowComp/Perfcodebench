#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // Skip initial 'r'
    if (*p == 'r') ++p;

    // Parse and discard id
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == ',') ++p;

    // Parse and discard a
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == ',') ++p;

    // Parse b
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      ++p;
    }
    if (*p == ',') ++p;

    // Parse c
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      ++p;
    }
    if (*p == ',') ++p;

    // Check flag for exact "hot"
    int hot = 0;
    if (*p == 'h' && *(p + 1) == 'o' && *(p + 2) == 't') {
      char next = *(p + 3);
      if (next == ',' || next == '\0') {
        hot = 1;
      }
    }

    uint64_t v = (uint64_t)(b + c + 28);
    if (hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
