#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int_fast(const char* p) {
  // Skip leading whitespace (to mirror atoi behavior conservatively)
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') {
    ++p;
  }
  int neg = 0;
  if (*p == '+' || *p == '-') {
    neg = (*p == '-');
    ++p;
  }
  unsigned int val = 0;
  unsigned char c;
  while ((c = (unsigned char)*p) >= '0' && c <= '9') {
    val = val * 10u + (unsigned int)(c - '0');
    ++p;
  }
  return neg ? -(int)val : (int)val;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    // First token: expected "u=..."
    // Find end of first token ('&')
    const char* p = s;
    while (*p != '&' && *p != '\0') {
      ++p;
    }
    const char* token2 = (*p == '&') ? (p + 1) : (p + 1); // assume well-formed like baseline

    // Second token: expected "score=..."
    const char* q = token2;
    while (*q != '&' && *q != '\0') {
      ++q;
    }
    const char* token3 = (*q == '&') ? (q + 1) : (q + 1);

    // Parse integers at known offsets
    int u = parse_int_fast(s + 2);
    int sc = parse_int_fast(token2 + 6);
    int f = parse_int_fast(token3 + 5);

    uint64_t v = (uint64_t)((u & 2047) + sc + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
