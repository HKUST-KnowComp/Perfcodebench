#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int is_digit_c(unsigned char c) {
  return (unsigned)(c - '0') < 10u;
}

static inline int is_upper_hex_c(unsigned char c) {
  return ((unsigned)(c - '0') < 10u) || ((unsigned)(c - 'A') < 6u);
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact format required by baseline:
       positions 0,1: any chars
       positions 2..5: 4 decimal digits
       position 6: '-'
       positions 7..14: 8 uppercase hex digits
       total length: 15
    */
    if (s[15] == '\0' &&
        is_digit_c(s[2]) && is_digit_c(s[3]) && is_digit_c(s[4]) && is_digit_c(s[5]) &&
        s[6] == '-' &&
        is_upper_hex_c(s[7]) && is_upper_hex_c(s[8]) && is_upper_hex_c(s[9]) && is_upper_hex_c(s[10]) &&
        is_upper_hex_c(s[11]) && is_upper_hex_c(s[12]) && is_upper_hex_c(s[13]) && is_upper_hex_c(s[14])) {
      v = (uint64_t)(s[2] + s[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
