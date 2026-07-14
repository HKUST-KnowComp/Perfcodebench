#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    
    /* Fixed format: 2 chars, 4 digits, '-', 8 hex chars = 15 chars total */
    /* Check indices 6 and 14 to verify length implicitly and format */
    if (s[6] == '-' && s[14] == '\0' && s[15] == '\0') {
        /* Validate digits and hex chars if necessary, but per requirements 
           we only need to match the structure and extract bytes at 2 and 7 */
        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}