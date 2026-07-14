#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int(const char** pp, int* out) {
  const char* p = *pp;
  int neg = 0;
  if (*p == '+' || *p == '-') {
    neg = (*p == '-');
    p++;
  }
  if ((unsigned)(*p - '0') > 9U) {
    return 0; // no digits
  }
  int val = (*p - '0');
  p++;
  while ((unsigned)(*p - '0') <= 9U) {
    val = val * 10 + (*p - '0');
    p++;
  }
  *out = neg ? -val : val;
  *pp = p;
  return 1;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];

    int b = 0;
    int c = 0;
    int is_hot = 0;
    int ok = 1;

    if (p == NULL || *p != 'r') {
      ok = 0;
    }

    if (ok) {
      p++; // skip 'r'
      int tmp;
      if (!parse_int(&p, &tmp)) { ok = 0; }
      if (ok && *p != ',') { ok = 0; }
      if (ok) { p++; }
      if (ok && !parse_int(&p, &tmp)) { ok = 0; } // a (unused)
      if (ok && *p != ',') { ok = 0; }
      if (ok) { p++; }
      if (ok && !parse_int(&p, &b)) { ok = 0; }
      if (ok && *p != ',') { ok = 0; }
      if (ok) { p++; }
      if (ok && !parse_int(&p, &c)) { ok = 0; }
      if (ok && *p != ',') { ok = 0; }
      if (ok) { p++; }
      if (ok) {
        // Check flag equals exactly "hot" and string ends.
        is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');
      }
    }

    // If parsing failed at any point, b and c remain 0 and is_hot remains 0,
    // matching behavior of the baseline when sscanf fails (no field assignments).
    uint64_t v = (uint64_t)((int)(b + c + 25));
    if (is_hot) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
