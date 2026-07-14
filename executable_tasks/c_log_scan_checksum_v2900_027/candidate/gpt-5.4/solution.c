#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /*
      Baseline accepts rows matching sscanf("%c%c%4u-%8X") with total length 15.
      That implies:
      - any first two chars
      - positions [2..5] are decimal digits
      - position [6] is '-'
      - positions [7..14] are hexadecimal digits accepted by %X
      - string ends at position 15

      On match, v = rows[i][2] + rows[i][7] as unsigned chars.
    */
    if (s[0] != 0 &&
        s[1] != 0 &&
        (unsigned)(s[2] - '0') <= 9u &&
        (unsigned)(s[3] - '0') <= 9u &&
        (unsigned)(s[4] - '0') <= 9u &&
        (unsigned)(s[5] - '0') <= 9u &&
        s[6] == '-' &&
        (((unsigned)(s[7] - '0') <= 9u) || ((unsigned)((s[7] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[8] - '0') <= 9u) || ((unsigned)((s[8] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[9] - '0') <= 9u) || ((unsigned)((s[9] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[10] - '0') <= 9u) || ((unsigned)((s[10] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[11] - '0') <= 9u) || ((unsigned)((s[11] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[12] - '0') <= 9u) || ((unsigned)((s[12] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[13] - '0') <= 9u) || ((unsigned)((s[13] | 32) - 'a') <= 5u)) &&
        (((unsigned)(s[14] - '0') <= 9u) || ((unsigned)((s[14] | 32) - 'a') <= 5u)) &&
        s[15] == 0) {
      v = (uint64_t)(s[2] + s[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
