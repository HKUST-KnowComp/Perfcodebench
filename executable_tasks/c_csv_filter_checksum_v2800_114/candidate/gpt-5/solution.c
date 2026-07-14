#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline const char* skip_int_field(const char* p) {
  char c = *p;
  if (c == '-' || c == '+') {
    c = *++p;
  }
  while ((unsigned)(c - '0') <= 9u) {
    c = *++p;
  }
  return p;
}

static inline const char* parse_int32(const char* p, int* out) {
  int neg = 0;
  int val = 0;
  char c = *p;
  if (c == '-') { neg = 1; c = *++p; }
  else if (c == '+') { c = *++p; }
  while ((unsigned)(c - '0') <= 9u) {
    val = val * 10 + (c - '0');
    c = *++p;
  }
  *out = neg ? -val : val;
  return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    if (*p == 'r') {
      ++p;
    }
    // Skip id
    p = skip_int_field(p);
    if (*p == ',') ++p;
    // Skip a
    p = skip_int_field(p);
    if (*p == ',') ++p;

    // Parse b
    int b = 0;
    p = parse_int32(p, &b);
    if (*p == ',') ++p;

    // Parse c
    int c = 0;
    p = parse_int32(p, &c);
    if (*p == ',') ++p;

    // Check flag == "hot" with termination at end or whitespace
    int is_hot = 0;
    if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
      char t = p[3];
      if (t == '\0' || t == ' ' || t == '\n' || t == '\r' || t == '\t') {
        is_hot = 1;
      }
    }

    int sum = b + c + 29;
    uint64_t v = (uint64_t)sum;
    if (is_hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
