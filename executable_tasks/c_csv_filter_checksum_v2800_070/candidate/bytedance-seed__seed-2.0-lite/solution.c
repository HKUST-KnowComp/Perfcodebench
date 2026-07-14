#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t FNV_MULT = 1099511628211ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];
    // Skip leading 'r' character
    p++;

    // Parse id (matches sscanf %d behavior, supports negatives)
    int id = 0;
    int sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
      id = id * 10 + (*p - '0');
      p++;
    }
    id *= sign;
    p++; // Skip trailing comma

    // Parse a
    int a = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
      a = a * 10 + (*p - '0');
      p++;
    }
    a *= sign;
    p++;

    // Parse b
    int b = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    b *= sign;
    p++;

    // Parse c
    int c = 0;
    sign = 1;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    c *= sign;
    p++; // Skip comma before flag

    // Calculate v, add 29 only if flag is exactly "hot" (matches strcmp behavior)
    uint64_t v = (uint64_t)(b + c + 8);
    if (*p == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == '\n' || p[3] == ',')) {
      v += 29ULL;
    }

    // Update FNV-1a hash exactly as original
    hash ^= (v + 1ULL);
    hash *= FNV_MULT;
  }
  return hash;
}