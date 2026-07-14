#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized query parameter scanner.
 * 
 * Performance improvements:
 * 1. Removed strcpy: No need to copy the input string to a local buffer.
 * 2. Removed strtok: Manual pointer advancement avoids the overhead of searching for delimiters multiple times.
 * 3. Removed atoi: Manual integer parsing is faster and doesn't require null-terminated substrings.
 * 4. Single-pass: The parser moves through each row string exactly once.
 */
uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Parse 'u' value: skip "u=" (2 chars)
    p += 2;
    int u = 0;
    while (*p >= '0' && *p <= '9') {
      u = u * 10 + (*p - '0');
      p++;
    }

    // Parse 'score' value: skip "&score=" (7 chars)
    p += 7;
    int s = 0;
    while (*p >= '0' && *p <= '9') {
      s = s * 10 + (*p - '0');
      p++;
    }

    // Parse 'flag' value: skip "&flag=" (6 chars)
    p += 6;
    int f = 0;
    while (*p >= '0' && *p <= '9') {
      f = f * 10 + (*p - '0');
      p++;
    }

    // Compute value and update FNV-1a hash
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }

  return hash;
}