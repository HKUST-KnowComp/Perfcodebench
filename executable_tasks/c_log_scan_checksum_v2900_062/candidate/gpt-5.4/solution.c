#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int is_digit_c(char c) {
  return (unsigned)(c - '0') <= 9u;
}

static inline int is_upper_hex_c(char c) {
  return ((unsigned)(c - '0') <= 9u) || ((unsigned)(c - 'A') <= 5u);
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /*
      Baseline semantics:
      sscanf(row, "%c%c%4u-%8X", ...) == 4 && strlen(row) == 15

      This means a match requires exactly:
      - any 2 chars
      - 1..4 decimal digits
      - '-'
      - 1..8 uppercase/decimal hex chars accepted by %X in the C locale
      - end of string at position 15 overall

      Since total length must be exactly 15, the variable-width fields are forced to:
      - 4 decimal digits at s[2..5]
      - '-' at s[6]
      - 8 hex digits at s[7..14]
      - '\0' at s[15]
    */
    if (s[15] == '\0' &&
        is_digit_c((char)s[2]) &&
        is_digit_c((char)s[3]) &&
        is_digit_c((char)s[4]) &&
        is_digit_c((char)s[5]) &&
        s[6] == '-' &&
        is_upper_hex_c((char)s[7]) &&
        is_upper_hex_c((char)s[8]) &&
        is_upper_hex_c((char)s[9]) &&
        is_upper_hex_c((char)s[10]) &&
        is_upper_hex_c((char)s[11]) &&
        is_upper_hex_c((char)s[12]) &&
        is_upper_hex_c((char)s[13]) &&
        is_upper_hex_c((char)s[14])) {
      v = (uint64_t)(s[2] + s[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
