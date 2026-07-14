#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    /* Expected format: XXYYYY-ZZZZZZZZ (15 chars total) */
    /* Check length and fixed delimiters */
    if (s[6] == '-' && s[14] == '\0') {
      /* Validate digits for x (4 chars) and y (8 chars) */
      int valid = 1;
      for (int j = 2; j < 6; ++j) if (s[j] < '0' || s[j] > '9') valid = 0;
      for (int j = 7; j < 15; ++j) {
        char c = s[j];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
          valid = 0;
      }
      if (valid) {
        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
      }
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}