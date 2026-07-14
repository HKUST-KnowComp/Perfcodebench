#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    uint64_t v = 0;
    
    /* Fixed format: 2 chars, 4 digits, '-', 8 hex chars, null terminator (15 bytes total) */
    /* Check length implicitly by verifying the null terminator at index 14 */
    if (s[14] == '\0' && s[6] == '-') {
      /* Validate digits at indices 2,3,4,5 and hex at 7-14 */
      int valid = 1;
      for (int j = 2; j < 6; ++j) if (s[j] < '0' || s[j] > '9') valid = 0;
      for (int j = 7; j < 15; ++j) {
        char c = s[j];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
          valid = 0;
          break;
        }
      }
      
      if (valid) {
        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
      }
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}