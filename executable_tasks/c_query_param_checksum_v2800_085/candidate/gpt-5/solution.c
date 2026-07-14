#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Parse first value: starts at p + 2 (e.g., "u=")
    p += 2;
    int u = 0;
    for (;;) {
      unsigned char c = (unsigned char)*p;
      if (c >= '0' && c <= '9') {
        u = u * 10 + (int)(c - '0');
        ++p;
      } else {
        break;
      }
    }

    // Move past '&' to start of second token
    if (*p == '&') ++p; // assume valid format

    // Second value starts at p + 6 (e.g., "score=")
    p += 6;
    int s = 0;
    for (;;) {
      unsigned char c = (unsigned char)*p;
      if (c >= '0' && c <= '9') {
        s = s * 10 + (int)(c - '0');
        ++p;
      } else {
        break;
      }
    }

    // Move past '&' to start of third token
    if (*p == '&') ++p; // assume valid format

    // Third value starts at p + 5 (e.g., "flag=")
    p += 5;
    int f = 0;
    for (;;) {
      unsigned char c = (unsigned char)*p;
      if (c >= '0' && c <= '9') {
        f = f * 10 + (int)(c - '0');
        ++p;
      } else {
        break;
      }
    }

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }

  return hash;
}
