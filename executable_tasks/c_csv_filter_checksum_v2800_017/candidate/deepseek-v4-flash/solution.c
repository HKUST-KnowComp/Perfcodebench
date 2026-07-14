#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // skip 'r'
    ++p;
    // parse id
    int id = 0;
    int sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    while (*p >= '0' && *p <= '9') {
      id = id * 10 + (*p - '0');
      ++p;
    }
    id *= sign;
    ++p; // ','
    // parse a
    int a = 0;
    sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    while (*p >= '0' && *p <= '9') {
      a = a * 10 + (*p - '0');
      ++p;
    }
    a *= sign;
    ++p; // ','
    // parse b
    int b = 0;
    sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    while (*p >= '0' && *p <= '9') {
      b = b * 10 + (*p - '0');
      ++p;
    }
    b *= sign;
    ++p; // ','
    // parse c
    int c = 0;
    sign = 1;
    if (*p == '-') { sign = -1; ++p; }
    while (*p >= '0' && *p <= '9') {
      c = c * 10 + (*p - '0');
      ++p;
    }
    c *= sign;
    ++p; // ','
    // check if flag equals "hot" exactly
    int flag_match = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');
    uint64_t v = (uint64_t)(b + c + 24);
    if (flag_match) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
