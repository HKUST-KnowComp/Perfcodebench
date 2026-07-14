#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // Skip leading 'r' character
    p++;
    // Skip unused id field to first comma
    while (*p != ',' && *p != '\0') p++;
    if (*p == ',') p++;
    // Skip unused a field to second comma
    while (*p != ',' && *p != '\0') p++;
    if (*p == ',') p++;
    // Parse required b value
    int b = 0;
    while (*p != ',' && *p != '\0') {
      b = b * 10 + (*p - '0');
      p++;
    }
    if (*p == ',') p++;
    // Parse required c value
    int c = 0;
    while (*p != ',' && *p != '\0') {
      c = c * 10 + (*p - '0');
      p++;
    }
    if (*p == ',') p++;
    // Calculate value same as original
    uint64_t v = (uint64_t)(b + c + 13);
    // Check for "hot" flag with direct byte compares, matches strcmp behavior
    if (*p == 'h' && *(p+1) == 'o' && *(p+2) == 't') {
      const char p3 = *(p+3);
      if (p3 == '\0' || p3 == '\n' || p3 == ',') {
        v += 29ULL;
      }
    }
    // Update hash identical to original implementation
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }
  return hash;
}