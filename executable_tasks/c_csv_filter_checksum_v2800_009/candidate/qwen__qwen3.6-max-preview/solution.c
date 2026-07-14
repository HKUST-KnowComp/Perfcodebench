#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // Skip 'r' and id
    while (*p != ',') p++;
    p++;
    // Skip a
    while (*p != ',') p++;
    p++;
    // Parse b
    while (*p == ' ') p++;
    int b = 0;
    int neg = (*p == '-');
    if (neg) p++;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      p++;
    }
    if (neg) b = -b;
    p++; // skip ','
    // Parse c
    while (*p == ' ') p++;
    int c = 0;
    neg = (*p == '-');
    if (neg) p++;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      p++;
    }
    if (neg) c = -c;
    p++; // skip ','
    // Check flag
    while (*p == ' ') p++;
    uint64_t v = (uint64_t)(b + c + 16);
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == '\n' || p[3] == '\r' || p[3] == ' ')) {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
