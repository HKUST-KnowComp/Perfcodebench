#include "interface.h"

#include <stdint.h>
#include <string.h>

static inline int is_hex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* ptr = rows[i];
    uint64_t v = 0;
    if (ptr[15] == '\0' && ptr[6] == '-') {
      int digits_valid = (ptr[2] >= '0' && ptr[2] <= '9') &&
                         (ptr[3] >= '0' && ptr[3] <= '9') &&
                         (ptr[4] >= '0' && ptr[4] <= '9') &&
                         (ptr[5] >= '0' && ptr[5] <= '9');
      int hex_valid = is_hex(ptr[7]) && is_hex(ptr[8]) && is_hex(ptr[9]) && is_hex(ptr[10]) &&
                      is_hex(ptr[11]) && is_hex(ptr[12]) && is_hex(ptr[13]) && is_hex(ptr[14]);
      if (digits_valid && hex_valid) {
        v = (uint64_t)((unsigned char)ptr[2] + (unsigned char)ptr[7]);
      }
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}