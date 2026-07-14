#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact baseline semantics:
       sscanf("%c%c%4u-%8X", ...) succeeds iff:
       - at least 2 chars exist for %c%c
       - chars [2..5] are decimal digits for %4u
       - char [6] is '-'
       - chars [7..14] are hex digits for %8X
       Then baseline additionally requires strlen(row) == 15.
       On success, v = (unsigned char)row[2] + (unsigned char)row[7].
    */
    if (s[0] != 0 && s[1] != 0) {
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
      unsigned char c15 = s[15];

      if ((unsigned)(c2 - '0') <= 9u &&
          (unsigned)(c3 - '0') <= 9u &&
          (unsigned)(c4 - '0') <= 9u &&
          (unsigned)(c5 - '0') <= 9u &&
          c6 == '-' &&
          (((unsigned)(c7 - '0') <= 9u) || ((unsigned)((c7 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c8 - '0') <= 9u) || ((unsigned)((c8 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c9 - '0') <= 9u) || ((unsigned)((c9 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c10 - '0') <= 9u) || ((unsigned)((c10 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c11 - '0') <= 9u) || ((unsigned)((c11 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c12 - '0') <= 9u) || ((unsigned)((c12 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c13 - '0') <= 9u) || ((unsigned)((c13 | 32) - 'a') <= 5u)) &&
          (((unsigned)(c14 - '0') <= 9u) || ((unsigned)((c14 | 32) - 'a') <= 5u)) &&
          c15 == 0) {
        v = (uint64_t)(c2 + c7);
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
