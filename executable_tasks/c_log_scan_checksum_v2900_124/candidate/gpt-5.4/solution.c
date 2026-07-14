#include "interface.h"

#include <stdint.h>
#include <stddef.h>

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
    if (s[15] == '\0') {
      unsigned char c2 = s[2];
      unsigned char c3 = s[3];
      unsigned char c4 = s[4];
      unsigned char c5 = s[5];
      unsigned char c6 = s[6];
      unsigned char c7 = s[7];
      unsigned char c8 = s[8];
      unsigned char c9 = s[9];
      unsigned char c10 = s[10];
      unsigned char c11 = s[11];
      unsigned char c12 = s[12];
      unsigned char c13 = s[13];
      unsigned char c14 = s[14];

      if ((unsigned)(c2 - '0') <= 9u &&
          (unsigned)(c3 - '0') <= 9u &&
          (unsigned)(c4 - '0') <= 9u &&
          (unsigned)(c5 - '0') <= 9u &&
          c6 == '-' &&
          (((unsigned)(c7 - '0') <= 9u) || ((unsigned)(c7 - 'A') <= 5u)) &&
          (((unsigned)(c8 - '0') <= 9u) || ((unsigned)(c8 - 'A') <= 5u)) &&
          (((unsigned)(c9 - '0') <= 9u) || ((unsigned)(c9 - 'A') <= 5u)) &&
          (((unsigned)(c10 - '0') <= 9u) || ((unsigned)(c10 - 'A') <= 5u)) &&
          (((unsigned)(c11 - '0') <= 9u) || ((unsigned)(c11 - 'A') <= 5u)) &&
          (((unsigned)(c12 - '0') <= 9u) || ((unsigned)(c12 - 'A') <= 5u)) &&
          (((unsigned)(c13 - '0') <= 9u) || ((unsigned)(c13 - 'A') <= 5u)) &&
          (((unsigned)(c14 - '0') <= 9u) || ((unsigned)(c14 - 'A') <= 5u))) {
        v = (uint64_t)(c2 + c7);
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
