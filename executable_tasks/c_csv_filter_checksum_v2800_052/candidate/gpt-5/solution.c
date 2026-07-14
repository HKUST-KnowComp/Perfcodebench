#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int_field(const char** pp) {
  const char* s = *pp;
  int neg = 0;
  if (*s == '-') {
    neg = 1;
    s++;
  } else if (*s == '+') {
    s++;
  }
  unsigned int x = 0;
  unsigned int d;
  while ((d = (unsigned int)((unsigned char)(*s) - (unsigned char)'0')) <= 9u) {
    x = x * 10u + d;
    s++;
  }
  if (*s == ',') {
    s++;
  }
  *pp = s;
  return neg ? -(int)x : (int)x;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    // Skip the id field (e.g., "r123") up to the first comma
    while (*p && *p != ',') {
      p++;
    }
    if (*p == ',') {
      p++; // move past comma to start of field 'a'
    }

    // Parse a, b, c fields
    (void)parse_int_field(&p); // a (ignored)
    int b = parse_int_field(&p);
    int c = parse_int_field(&p);

    // p now points to start of flag
    unsigned int is_hot = 0;
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (unsigned char)p[3] == '\0') {
      is_hot = 1;
    }

    int sum = b + c + 13; // int arithmetic as in baseline
    uint64_t v = (uint64_t)sum;
    if (is_hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
