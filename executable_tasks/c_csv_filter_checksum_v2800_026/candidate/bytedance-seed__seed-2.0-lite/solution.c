#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];
    // Skip leading 'r' character
    p++;

    // Parse ID field
    int id = 0;
    while (*p != ',') {
      id = id * 10 + (*p - '0');
      p++;
    }
    p++; // Skip delimiter comma

    // Parse 'a' integer field
    int a = 0;
    while (*p != ',') {
      a = a * 10 + (*p - '0');
      p++;
    }
    p++;

    // Parse 'b' integer field
    int b = 0;
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;

    // Parse 'c' integer field
    int c = 0;
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;

    // Calculate value matching original logic
    uint64_t v = (uint64_t)(b + c + 10);
    // Direct character check for "hot" flag, replaces strcmp
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',' || p[3] == '\n')) {
      v += 29ULL;
    }

    // Update FNV-1a hash exactly as original implementation
    hash ^= (v + 1ULL);
    hash *= fnv_prime;
  }
  return hash;
}