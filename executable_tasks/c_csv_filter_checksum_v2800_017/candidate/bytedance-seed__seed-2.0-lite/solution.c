#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char *p = rows[i];
    // Skip leading 'r' and unused id field
    while(*p != ',') p++;
    p++;
    // Skip unused a field
    while(*p != ',') p++;
    p++;
    // Parse used b value
    int b = 0;
    while(*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;
    // Parse used c value
    int c = 0;
    while(*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;
    // Check for "hot" flag with direct byte comparison
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ','));
    // Exact same value and hash calculation as original
    uint64_t v = (uint64_t)(b + c + 24);
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}