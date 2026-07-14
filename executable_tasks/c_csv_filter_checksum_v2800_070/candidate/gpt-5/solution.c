#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline void skip_field(const char** pp) {
  const char* p = *pp;
  while (*p && *p != ',') ++p;
  if (*p == ',') ++p;
  *pp = p;
}

static inline int parse_int_field(const char** pp) {
  const char* p = *pp;
  int neg = 0;
  if (*p == '-') { neg = 1; ++p; }
  else if (*p == '+') { ++p; }
  int v = 0;
  unsigned char c;
  while ((c = (unsigned char)*p) != '\0' && c != ',') {
    v = v * 10 + (int)(c - '0');
    ++p;
  }
  if (*p == ',') ++p;
  *pp = p;
  return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET;

  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    if (*p == 'r') ++p; // skip leading 'r'

    // Skip id and a
    skip_field(&p);
    skip_field(&p);

    // Parse b and c
    int b = parse_int_field(&p);
    int c = parse_int_field(&p);

    // p now points to flag
    int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0'));

    int sum = b + c + 8; // keep int semantics as in baseline
    uint64_t v = (uint64_t)sum;
    if (is_hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= FNV_PRIME;
  }

  return hash;
}
