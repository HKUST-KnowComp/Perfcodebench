#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static inline const char* skip_to_next_comma(const char* p) {
  while (*p && *p != ',') p++;
  if (*p == ',') ++p;
  return p;
}

static inline int parse_int(const char** pp) {
  const char* p = *pp;
  int sign = 1;
  if (*p == '-') { sign = -1; ++p; }
  else if (*p == '+') { ++p; }
  int v = 0;
  unsigned char ch;
  while ((ch = (unsigned char)*p), (unsigned)(ch - (unsigned char)'0') <= 9u) {
    v = v * 10 + (int)(ch - (unsigned char)'0');
    ++p;
  }
  *pp = p;
  return sign * v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Skip the first two fields: "r<id>", and "<a>"
    p = skip_to_next_comma(p);  // after id
    p = skip_to_next_comma(p);  // after a

    // Parse b
    int b = parse_int(&p);
    if (*p == ',') ++p;

    // Parse c
    int c = parse_int(&p);
    if (*p == ',') ++p;

    // Skip leading whitespace before flag (mimic %s behavior)
    while (*p && (unsigned char)*p <= ' ') ++p;

    // Check for exact flag "hot" followed by end or whitespace
    uint64_t v = (uint64_t)((b + c + 11));
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      unsigned char t = (unsigned char)p[3];
      if (t == 0 || t <= ' ') {
        v += 29ULL;
      }
    }

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
