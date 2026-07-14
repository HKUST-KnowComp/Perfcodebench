#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];
    // Skip unused id field to first comma
    while (*p != ',') p++;
    p++;
    // Skip unused a field to second comma
    while (*p != ',') p++;
    p++;
    // Parse only used b value
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;
    // Parse only used c value
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;
    // Calculate v matching original logic
    uint64_t v = (uint64_t)(b + c + 29);
    // Directly check for "hot" flag without strcmp
    if (*p == 'h' && *(p+1) == 'o' && *(p+2) == 't' && (*(p+3) == '\0' || *(p+3) == ',')) {
      v += 29ULL;
    }
    // Update FNV-1a hash exactly as original
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}