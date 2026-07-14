#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const unsigned char* s = (const unsigned char*)rows[i];
    uint64_t v = 0;

    /* Exact fixed-format validation for: %c%c%4u-%8X and total length 15 */
    if (s[0] != 0 &&
        s[1] != 0 &&
        s[2] >= '0' && s[2] <= '9' &&
        s[3] >= '0' && s[3] <= '9' &&
        s[4] >= '0' && s[4] <= '9' &&
        s[5] >= '0' && s[5] <= '9' &&
        s[6] == '-' &&
        ((s[7] >= '0' && s[7] <= '9') || (s[7] >= 'A' && s[7] <= 'F')) &&
        ((s[8] >= '0' && s[8] <= '9') || (s[8] >= 'A' && s[8] <= 'F')) &&
        ((s[9] >= '0' && s[9] <= '9') || (s[9] >= 'A' && s[9] <= 'F')) &&
        ((s[10] >= '0' && s[10] <= '9') || (s[10] >= 'A' && s[10] <= 'F')) &&
        ((s[11] >= '0' && s[11] <= '9') || (s[11] >= 'A' && s[11] <= 'F')) &&
        ((s[12] >= '0' && s[12] <= '9') || (s[12] >= 'A' && s[12] <= 'F')) &&
        ((s[13] >= '0' && s[13] <= '9') || (s[13] >= 'A' && s[13] <= 'F')) &&
        ((s[14] >= '0' && s[14] <= '9') || (s[14] >= 'A' && s[14] <= 'F')) &&
        s[15] == 0) {
      v = (uint64_t)(s[2] + s[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
