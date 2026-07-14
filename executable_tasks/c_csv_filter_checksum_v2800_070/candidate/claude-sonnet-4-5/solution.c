#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Skip "r" and parse id (not used, but must parse)
    p++;
    while (*p >= '0' && *p <= '9') p++;
    p++; // skip comma
    
    // Parse a (not used)
    int neg = (*p == '-');
    if (neg) p++;
    while (*p >= '0' && *p <= '9') p++;
    p++; // skip comma
    
    // Parse b
    neg = (*p == '-');
    if (neg) p++;
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    if (neg) b = -b;
    p++; // skip comma
    
    // Parse c
    neg = (*p == '-');
    if (neg) p++;
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    if (neg) c = -c;
    p++; // skip comma
    
    // Check flag - compare as 32-bit integer for "hot\0"
    uint64_t v = (uint64_t)(b + c + 8);
    uint32_t flag_word;
    memcpy(&flag_word, p, 4);
    if (flag_word == 0x00746F68) { // "hot\0" in little-endian
      v += 29ULL;
    }
    
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}
