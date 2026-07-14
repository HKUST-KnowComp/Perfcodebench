#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Skip first field ("r<id>") up to first comma
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    // Skip second field (<a>) up to second comma
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;

    // Parse third field: b
    int b = 0;
    int neg = 0;
    if (*p == '-') { neg = 1; ++p; }
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      ++p;
    }
    if (neg) b = -b;
    if (*p == ',') ++p;

    // Parse fourth field: c
    int c = 0;
    neg = 0;
    if (*p == '-') { neg = 1; ++p; }
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      ++p;
    }
    if (neg) c = -c;
    if (*p == ',') ++p; // move to start of flag

    // Check flag equals "hot" exactly
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');

    // Compute v and update hash (preserve int addition semantics before cast)
    int sum_ic = b + c + 7;
    uint64_t v = (uint64_t)sum_ic;
    if (is_hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }

  return hash;
}
