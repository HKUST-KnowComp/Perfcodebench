#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact baseline acceptance:
       sscanf("%c%c%4u-%8X") must match 4 items and strlen must be 15.
       With fixed length 15, this means:
       - bytes [0],[1] exist (always true for length 15)
       - bytes [2..5] are decimal digits
       - byte  [6] is '-'
       - bytes [7..14] are hexadecimal digits (upper/lower accepted by %X)
       - byte  [15] is '\0'
       The parsed numeric values are irrelevant to the checksum.
    */
    if (s[15] == '\0') {
      unsigned c2 = s[2] - (unsigned char)'0';
      unsigned c3 = s[3] - (unsigned char)'0';
      unsigned c4 = s[4] - (unsigned char)'0';
      unsigned c5 = s[5] - (unsigned char)'0';

      if (c2 <= 9u && c3 <= 9u && c4 <= 9u && c5 <= 9u && s[6] == (unsigned char)'-') {
        unsigned char h7 = s[7];
        unsigned char h8 = s[8];
        unsigned char h9 = s[9];
        unsigned char h10 = s[10];
        unsigned char h11 = s[11];
        unsigned char h12 = s[12];
        unsigned char h13 = s[13];
        unsigned char h14 = s[14];

        #define HEX_OK(ch) ((((unsigned)((ch) - (unsigned char)'0')) <= 9u) || \
                            (((unsigned)(((ch) | 32u) - (unsigned char)'a')) <= 5u))

        if (HEX_OK(h7) && HEX_OK(h8) && HEX_OK(h9) && HEX_OK(h10) &&
            HEX_OK(h11) && HEX_OK(h12) && HEX_OK(h13) && HEX_OK(h14)) {
          v = (uint64_t)(s[2] + s[7]);
        }

        #undef HEX_OK
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
