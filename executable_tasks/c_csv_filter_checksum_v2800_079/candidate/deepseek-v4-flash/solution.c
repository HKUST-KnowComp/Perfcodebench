#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // skip leading 'r'
    if (*p == 'r') ++p;

    // skip id
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == ',') ++p;

    // skip a
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == ',') ++p;

    // parse b
    int b = 0;
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      ++p;
    }
    if (*p == ',') ++p;

    // parse c
    int c = 0;
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      ++p;
    }
    if (*p == ',') ++p;

    // check if flag equals "hot"
    int hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' &&
               (p[3] == '\0' || p[3] == '\n' || p[3] == '\r'));

    uint64_t v = (uint64_t)(b + c + 17);
    if (hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
