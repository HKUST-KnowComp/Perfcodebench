#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline unsigned hex_val(unsigned char c) {
  if ((unsigned)(c - '0') <= 9u) return (unsigned)(c - '0');
  c = (unsigned char)(c | 32u);
  if ((unsigned)(c - 'a') <= 5u) return (unsigned)(c - 'a') + 10u;
  return 16u;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    if (s[0] != 0 && s[1] != 0 &&
        (unsigned)(s[2] - '0') <= 9u &&
        (unsigned)(s[3] - '0') <= 9u &&
        (unsigned)(s[4] - '0') <= 9u &&
        (unsigned)(s[5] - '0') <= 9u &&
        s[6] == '-') {
      unsigned h0 = hex_val(s[7]);
      unsigned h1 = hex_val(s[8]);
      unsigned h2 = hex_val(s[9]);
      unsigned h3 = hex_val(s[10]);
      unsigned h4 = hex_val(s[11]);
      unsigned h5 = hex_val(s[12]);
      unsigned h6 = hex_val(s[13]);
      unsigned h7 = hex_val(s[14]);

      if (h0 < 16u && h1 < 16u && h2 < 16u && h3 < 16u &&
          h4 < 16u && h5 < 16u && h6 < 16u && h7 < 16u &&
          s[15] == 0) {
        v = (uint64_t)(s[2] + s[7]);
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
