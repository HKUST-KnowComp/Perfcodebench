#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact format/length check equivalent to:
       sscanf(row, "%c%c%4u-%8X", ...) == 4 && strlen(row) == 15
       Format positions:
       0,1: any chars
       2..5: 4 decimal digits
       6: '-'
       7..14: 8 uppercase/lowercase hex digits
       15: '\0'
    */
    if (s[15] == '\0') {
      unsigned c2 = (unsigned)(s[2] - '0');
      unsigned c3 = (unsigned)(s[3] - '0');
      unsigned c4 = (unsigned)(s[4] - '0');
      unsigned c5 = (unsigned)(s[5] - '0');

      if (c2 < 10u && c3 < 10u && c4 < 10u && c5 < 10u && s[6] == '-') {
        unsigned char h7  = s[7];
        unsigned char h8  = s[8];
        unsigned char h9  = s[9];
        unsigned char h10 = s[10];
        unsigned char h11 = s[11];
        unsigned char h12 = s[12];
        unsigned char h13 = s[13];
        unsigned char h14 = s[14];

        unsigned ok = 1u;
        ok &= (unsigned)((h7  >= '0' && h7  <= '9') || (h7  >= 'A' && h7  <= 'F') || (h7  >= 'a' && h7  <= 'f'));
        ok &= (unsigned)((h8  >= '0' && h8  <= '9') || (h8  >= 'A' && h8  <= 'F') || (h8  >= 'a' && h8  <= 'f'));
        ok &= (unsigned)((h9  >= '0' && h9  <= '9') || (h9  >= 'A' && h9  <= 'F') || (h9  >= 'a' && h9  <= 'f'));
        ok &= (unsigned)((h10 >= '0' && h10 <= '9') || (h10 >= 'A' && h10 <= 'F') || (h10 >= 'a' && h10 <= 'f'));
        ok &= (unsigned)((h11 >= '0' && h11 <= '9') || (h11 >= 'A' && h11 <= 'F') || (h11 >= 'a' && h11 <= 'f'));
        ok &= (unsigned)((h12 >= '0' && h12 <= '9') || (h12 >= 'A' && h12 <= 'F') || (h12 >= 'a' && h12 <= 'f'));
        ok &= (unsigned)((h13 >= '0' && h13 <= '9') || (h13 >= 'A' && h13 <= 'F') || (h13 >= 'a' && h13 <= 'f'));
        ok &= (unsigned)((h14 >= '0' && h14 <= '9') || (h14 >= 'A' && h14 <= 'F') || (h14 >= 'a' && h14 <= 'f'));

        if (ok) {
          v = (uint64_t)(s[2] + s[7]);
        }
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
