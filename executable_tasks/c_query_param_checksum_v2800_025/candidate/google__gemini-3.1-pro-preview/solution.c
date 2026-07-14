#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized query parameter scanner.
 * 
 * Performance improvements:
 * 1. Removed strcpy: Operates directly on the input strings.
 * 2. Removed strtok: Replaced with manual pointer arithmetic to find delimiters.
 * 3. Removed atoi: Implemented a fast, inline integer parser that stops at delimiters.
 * 4. Single-pass: Each row is traversed exactly once.
 */
uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Parse 'u' - starts at index 2 (after "u=")
    p += 2;
    int u = 0;
    while (*p != '&') {
      u = u * 10 + (*p - '0');
      p++;
    }

    // Parse 'score' - starts after "&score=" (7 chars)
    p += 7;
    int s = 0;
    while (*p != '&') {
      s = s * 10 + (*p - '0');
      p++;
    }

    // Parse 'flag' - starts after "&flag=" (6 chars)
    p += 6;
    int f = 0;
    while (*p >= '0' && *p <= '9') {
      f = f * 10 + (*p - '0');
      p++;
    }

    // Compute hash update using the provided logic
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }

  return hash;
}