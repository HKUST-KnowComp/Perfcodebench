#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    uint64_t v = 0;
    if (row[15] == '\0' && row[6] == '-') {
      _Bool valid = 1;
      for (int j = 2; j < 6; j++) {
        if (!isdigit((unsigned char)row[j])) {
          valid = 0;
          break;
        }
      }
      if (valid) {
        for (int j = 7; j < 15; j++) {
          if (!isxdigit((unsigned char)row[j])) {
            valid = 0;
            break;
          }
        }
      }
      if (valid) {
        v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
      }
    }
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }
  return hash;
}