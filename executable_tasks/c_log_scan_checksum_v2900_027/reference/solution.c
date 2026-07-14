#include "interface.h"

#include <stdbool.h>
#include <stdint.h>

static inline bool is_hex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    int ok = s[0] >= 'A' && s[0] <= 'Z' &&
             s[1] >= 'A' && s[1] <= 'Z' &&
             s[2] >= '0' && s[2] <= '9' &&
             s[3] >= '0' && s[3] <= '9' &&
             s[4] >= '0' && s[4] <= '9' &&
             s[5] >= '0' && s[5] <= '9' &&
             s[6] == '-' &&
             is_hex(s[7]) && is_hex(s[8]) && is_hex(s[9]) && is_hex(s[10]) &&
             is_hex(s[11]) && is_hex(s[12]) && is_hex(s[13]) && is_hex(s[14]) &&
             s[15] == 0;
    uint64_t v = ok ? (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]) : 0ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
