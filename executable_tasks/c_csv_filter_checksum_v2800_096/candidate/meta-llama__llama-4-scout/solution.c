/*
 * Optimized C CSV row filtering and checksum calculation.
 *
 * This code scans many CSV rows in C and returns an exact checksum over filtered fields.
 */
#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define HOT_FLAG "hot"
#define HOT_FLAG_LEN 3

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    int id, a, b, c;
    char flag[8];
    // Use sscanf with a more efficient format string
    if (sscanf(rows[i], "r%d,%d,%d,%d,%7s", &id, &a, &b, &c, flag) != 5) {
      // Handle incorrect input format
      continue;
    }
    uint64_t v = (uint64_t)(b + c + 11);
    // Compare flag with HOT_FLAG using memcmp for efficiency
    if (memcmp(flag, HOT_FLAG, HOT_FLAG_LEN) == 0 && flag[HOT_FLAG_LEN] == '\0') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}