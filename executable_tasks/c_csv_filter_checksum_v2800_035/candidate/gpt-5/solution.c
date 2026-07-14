#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int parse_int_csv(const char **pp) {
  const char *s = *pp;
  int neg = 0;
  if (*s == '-') { neg = 1; ++s; }
  else if (*s == '+') { ++s; }
  int v = 0;
  unsigned d;
  while ((d = (unsigned)((unsigned char)*s - (unsigned char)'0')) <= 9u) {
    v = v * 10 + (int)d;
    ++s;
  }
  if (neg) v = -v;
  if (*s == ',') ++s;
  *pp = s;
  return v;
}

static inline int is_hot_flag(const char *p) {
  // Check for exact "hot" followed by end or whitespace
  if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
    char t = p[3];
    if (t == '\0' || t == '\n' || t == '\r' || t == ' ' || t == '\t' || t == '\v' || t == '\f') {
      return 1;
    }
  }
  return 0;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  const char* const* it = rows;
  const char* const* end = rows + row_count;
  for (; it != end; ++it) {
    const char *p = *it;
    // Expect leading 'r'
    if (*p == 'r') ++p;
    // id
    (void)parse_int_csv(&p);
    // a
    (void)parse_int_csv(&p);
    // b
    int b = parse_int_csv(&p);
    // c
    int c = parse_int_csv(&p);
    // flag is the rest of the line; check if exactly "hot"
    int hot = is_hot_flag(p);

    int sum = b + c + 19; // match baseline int arithmetic before cast
    uint64_t v = (uint64_t)sum;
    if (hot) v += 29ULL;

    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
