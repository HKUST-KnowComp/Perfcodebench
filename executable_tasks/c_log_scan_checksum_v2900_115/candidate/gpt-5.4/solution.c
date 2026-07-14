#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact fixed-format check for: %c%c%4u-%8X and total length 15 */
    if (s[15] == '\0' && s[6] == '-') {
      unsigned c2 = s[2];
      unsigned c3 = s[3];
      unsigned c4 = s[4];
      unsigned c5 = s[5];

      if ((unsigned)(c2 - '0') < 10u &&
          (unsigned)(c3 - '0') < 10u &&
          (unsigned)(c4 - '0') < 10u &&
          (unsigned)(c5 - '0') < 10u) {
        unsigned h7  = s[7];
        unsigned h8  = s[8];
        unsigned h9  = s[9];
        unsigned h10 = s[10];
        unsigned h11 = s[11];
        unsigned h12 = s[12];
        unsigned h13 = s[13];
        unsigned h14 = s[14];

        unsigned ok = 1u;
        ok &= (((h7  >= '0' && h7  <= '9') || (h7  >= 'A' && h7  <= 'F')) ? 1u : 0u);
        ok &= (((h8  >= '0' && h8  <= '9') || (h8  >= 'A' && h8  <= 'F')) ? 1u : 0u);
        ok &= (((h9  >= '0' && h9  <= '9') || (h9  >= 'A' && h9  <= 'F')) ? 1u : 0u);
        ok &= (((h10 >= '0' && h10 <= '9') || (h10 >= 'A' && h10 <= 'F')) ? 1u : 0u);
        ok &= (((h11 >= '0' && h11 <= '9') || (h11 >= 'A' && h11 <= 'F')) ? 1u : 0u);
        ok &= (((h12 >= '0' && h12 <= '9') || (h12 >= 'A' && h12 <= 'F')) ? 1u : 0u);
        ok &= (((h13 >= '0' && h13 <= '9') || (h13 >= 'A' && h13 <= 'F')) ? 1u : 0u);
        ok &= (((h14 >= '0' && h14 <= '9') || (h14 >= 'A' && h14 <= 'F')) ? 1u : 0u);

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
