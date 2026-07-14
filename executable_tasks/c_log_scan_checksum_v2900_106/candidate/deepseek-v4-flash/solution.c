#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *s = rows[i];
    // Quick length check: ensure at least 15 chars and null terminator at index 15
    if (!s || s[0] == '\0' || s[14] == '\0' || s[15] != '\0') continue;
    // Hyphen at position 6
    if (s[6] != '-') continue;
    // Positions 2-5 must be decimal digits
    if (s[2] < '0' || s[2] > '9') continue;
    if (s[3] < '0' || s[3] > '9') continue;
    if (s[4] < '0' || s[4] > '9') continue;
    if (s[5] < '0' || s[5] > '9') continue;
    // Positions 7-14 must be hex digits
    for (int j = 7; j <= 14; ++j) {
      char c = s[j];
      if (!((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F'))) {
        goto skip;  // Not a valid hex digit
      }
    }
    // Compute v as sum of ASCII values at indices 2 and 7
    uint64_t v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
skip:;
  }
  return hash;
}
