#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact fixed-format validation for: %c%c%4u-%8X and total length 15
       Positions:
       0,1: any chars
       2..5: decimal digits
       6: '-'
       7..14: uppercase hex digits
       15: '\0'
    */
    if (s[15] == '\0' &&
        s[6] == '-' &&
        (unsigned)(s[2] - '0') <= 9u &&
        (unsigned)(s[3] - '0') <= 9u &&
        (unsigned)(s[4] - '0') <= 9u &&
        (unsigned)(s[5] - '0') <= 9u &&
        (((unsigned)(s[7] - '0') <= 9u) || ((unsigned)(s[7] - 'A') <= 5u)) &&
        (((unsigned)(s[8] - '0') <= 9u) || ((unsigned)(s[8] - 'A') <= 5u)) &&
        (((unsigned)(s[9] - '0') <= 9u) || ((unsigned)(s[9] - 'A') <= 5u)) &&
        (((unsigned)(s[10] - '0') <= 9u) || ((unsigned)(s[10] - 'A') <= 5u)) &&
        (((unsigned)(s[11] - '0') <= 9u) || ((unsigned)(s[11] - 'A') <= 5u)) &&
        (((unsigned)(s[12] - '0') <= 9u) || ((unsigned)(s[12] - 'A') <= 5u)) &&
        (((unsigned)(s[13] - '0') <= 9u) || ((unsigned)(s[13] - 'A') <= 5u)) &&
        (((unsigned)(s[14] - '0') <= 9u) || ((unsigned)(s[14] - 'A') <= 5u))) {
      v = (uint64_t)(s[2] + s[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
