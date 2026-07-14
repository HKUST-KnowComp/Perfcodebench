#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* r = rows[i];
    uint64_t v = 0;
    /* Format: CCdddd-XXXXXXXX (15 chars) */
    /* Check length == 15 by verifying char at 15 is null */
    if (r[0] && r[1] && r[2] && r[3] && r[4] && r[5] && r[6] && r[7] &&
        r[8] && r[9] && r[10] && r[11] && r[12] && r[13] && r[14] && !r[15]) {
      /* Validate 4 decimal digits at positions 2-5 */
      unsigned char c2 = (unsigned char)r[2];
      unsigned char c3 = (unsigned char)r[3];
      unsigned char c4 = (unsigned char)r[4];
      unsigned char c5 = (unsigned char)r[5];
      int dec_ok = (c2 >= '0' && c2 <= '9') &&
                   (c3 >= '0' && c3 <= '9') &&
                   (c4 >= '0' && c4 <= '9') &&
                   (c5 >= '0' && c5 <= '9');
      /* Validate dash at position 6 */
      int dash_ok = (r[6] == '-');
      /* Validate 8 hex digits at positions 7-14 */
      unsigned char c7 = (unsigned char)r[7];
      unsigned char c8 = (unsigned char)r[8];
      unsigned char c9 = (unsigned char)r[9];
      unsigned char c10 = (unsigned char)r[10];
      unsigned char c11 = (unsigned char)r[11];
      unsigned char c12 = (unsigned char)r[12];
      unsigned char c13 = (unsigned char)r[13];
      unsigned char c14 = (unsigned char)r[14];
      int hex_ok = ((c7 >= '0' && c7 <= '9') || (c7 >= 'A' && c7 <= 'F') || (c7 >= 'a' && c7 <= 'f')) &&
                   ((c8 >= '0' && c8 <= '9') || (c8 >= 'A' && c8 <= 'F') || (c8 >= 'a' && c8 <= 'f')) &&
                   ((c9 >= '0' && c9 <= '9') || (c9 >= 'A' && c9 <= 'F') || (c9 >= 'a' && c9 <= 'f')) &&
                   ((c10 >= '0' && c10 <= '9') || (c10 >= 'A' && c10 <= 'F') || (c10 >= 'a' && c10 <= 'f')) &&
                   ((c11 >= '0' && c11 <= '9') || (c11 >= 'A' && c11 <= 'F') || (c11 >= 'a' && c11 <= 'f')) &&
                   ((c12 >= '0' && c12 <= '9') || (c12 >= 'A' && c12 <= 'F') || (c12 >= 'a' && c12 <= 'f')) &&
                   ((c13 >= '0' && c13 <= '9') || (c13 >= 'A' && c13 <= 'F') || (c13 >= 'a' && c13 <= 'f')) &&
                   ((c14 >= '0' && c14 <= '9') || (c14 >= 'A' && c14 <= 'F') || (c14 >= 'a' && c14 <= 'f'));
      if (dec_ok && dash_ok && hex_ok) {
        v = (uint64_t)(c2 + c7);
      }
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}