#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // Skip unused id and a fields by counting past first two commas
    int commas = 0;
    while (commas < 2) {
      if (*p == ',') commas++;
      p++;
    }
    // Parse b (only used numeric field after a)
    int b = 0;
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++; // Skip comma separating b and c
    // Parse c
    int c = 0;
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++; // Skip comma leading to flag
    // Calculate v exactly as original
    uint64_t v = (uint64_t)(b + c + 16);
    // Exact strcmp("hot") replacement with no function call
    if (*p == 'h' && *(p+1) == 'o' && *(p+2) == 't' && *(p+3) == '\0') {
      v += 29ULL;
    }
    // Update FNV-1a hash identical to original implementation
    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }
  return hash;
}