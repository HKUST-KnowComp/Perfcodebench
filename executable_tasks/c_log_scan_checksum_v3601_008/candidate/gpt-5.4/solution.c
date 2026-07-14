#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact length check for 15-byte string. */
    if (s[15] == '\0') {
      /* Format: %c%c%4u-%8X */
      if (s[6] == '-') {
        /* %4u: exactly 4 decimal digits */
        unsigned c2 = s[2] - '0';
        unsigned c3 = s[3] - '0';
        unsigned c4 = s[4] - '0';
        unsigned c5 = s[5] - '0';

        if (c2 < 10u && c3 < 10u && c4 < 10u && c5 < 10u) {
          /* %8X: exactly 8 hexadecimal digits */
          unsigned char h7  = s[7];
          unsigned char h8  = s[8];
          unsigned char h9  = s[9];
          unsigned char h10 = s[10];
          unsigned char h11 = s[11];
          unsigned char h12 = s[12];
          unsigned char h13 = s[13];
          unsigned char h14 = s[14];

          unsigned ok = 1u;
          ok &= (unsigned)((h7  >= '0' && h7  <= '9') || (h7  >= 'A' && h7  <= 'F'));
          ok &= (unsigned)((h8  >= '0' && h8  <= '9') || (h8  >= 'A' && h8  <= 'F'));
          ok &= (unsigned)((h9  >= '0' && h9  <= '9') || (h9  >= 'A' && h9  <= 'F'));
          ok &= (unsigned)((h10 >= '0' && h10 <= '9') || (h10 >= 'A' && h10 <= 'F'));
          ok &= (unsigned)((h11 >= '0' && h11 <= '9') || (h11 >= 'A' && h11 <= 'F'));
          ok &= (unsigned)((h12 >= '0' && h12 <= '9') || (h12 >= 'A' && h12 <= 'F'));
          ok &= (unsigned)((h13 >= '0' && h13 <= '9') || (h13 >= 'A' && h13 <= 'F'));
          ok &= (unsigned)((h14 >= '0' && h14 <= '9') || (h14 >= 'A' && h14 <= 'F'));

          if (ok) {
            v = (uint64_t)(s[2] + s[7]);
          }
        }
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
