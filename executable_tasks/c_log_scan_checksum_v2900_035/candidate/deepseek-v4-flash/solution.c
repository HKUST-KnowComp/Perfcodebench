#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    int matched = 0;
    // Check four digits at positions 2-5
    for (int j = 0; j < 4; ++j) {
      char c = s[2 + j];
      if (c < '0' || c > '9') goto skip;
    }
    if (s[6] != '-') goto skip;
    // Check eight hex digits at positions 7-14
    for (int j = 0; j < 8; ++j) {
      char c = s[7 + j];
      if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
        goto skip;
    }
    if (s[15] != '\0') goto skip;
    matched = 1;
skip:
    uint64_t v = 0;
    if (matched)
      v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}