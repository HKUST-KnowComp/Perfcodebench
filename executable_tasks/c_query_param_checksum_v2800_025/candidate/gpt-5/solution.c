#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_int_range(const char* p, const char* end) {
  // Minimal atoi-like parser: optional sign, then digits until non-digit or end
  int neg = 0;
  if (p < end) {
    char c = *p;
    if (c == '-') { neg = 1; ++p; }
    else if (c == '+') { ++p; }
  }
  unsigned int r = 0;
  while (p < end) {
    unsigned char d = (unsigned char)(*p - '0');
    if (d > 9) break;
    r = r * 10u + d;
    ++p;
  }
  return neg ? -(int)r : (int)r;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];

    // Find end of first token
    const char* a = row;
    const char* a_end = a;
    for (; *a_end != '&' && *a_end != '\0'; ++a_end) {}

    // Second token start/end
    const char* b = (*a_end == '&') ? (a_end + 1) : a_end;
    const char* b_end = b;
    for (; *b_end != '&' && *b_end != '\0'; ++b_end) {}

    // Third token start/end
    const char* c = (*b_end == '&') ? (b_end + 1) : b_end;
    const char* c_end = c;
    for (; *c_end != '&' && *c_end != '\0'; ++c_end) {}

    // Offsets inside tokens: +2, +6, +5 as in baseline
    const char* u_ptr = a + 2;
    if (u_ptr > a_end) u_ptr = a_end; // safety, though inputs are expected valid
    const char* s_ptr = b + 6;
    if (s_ptr > b_end) s_ptr = b_end;
    const char* f_ptr = c + 5;
    if (f_ptr > c_end) f_ptr = c_end;

    int u = parse_int_range(u_ptr, a_end);
    int s = parse_int_range(s_ptr, b_end);
    int f = parse_int_range(f_ptr, c_end);

    int tmp = (u & 2047) + s + f * 19;
    uint64_t v = (uint64_t)tmp;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
