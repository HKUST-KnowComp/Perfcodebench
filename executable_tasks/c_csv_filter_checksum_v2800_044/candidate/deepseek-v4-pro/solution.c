#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];

    /* skip 'r' */
    if (*s == 'r') s++;

    /* skip id (first integer) */
    if (*s == '-') s++;
    while (*s >= '0' && *s <= '9') s++;
    if (*s == ',') s++;

    /* skip a (second integer) */
    if (*s == '-') s++;
    while (*s >= '0' && *s <= '9') s++;
    if (*s == ',') s++;

    /* parse b (third integer) */
    int b_sign = 1;
    if (*s == '-') { b_sign = -1; s++; }
    int b = 0;
    while (*s >= '0' && *s <= '9') {
      b = b * 10 + (*s - '0');
      s++;
    }
    b *= b_sign;
    if (*s == ',') s++;

    /* parse c (fourth integer) */
    int c_sign = 1;
    if (*s == '-') { c_sign = -1; s++; }
    int c = 0;
    while (*s >= '0' && *s <= '9') {
      c = c * 10 + (*s - '0');
      s++;
    }
    c *= c_sign;
    if (*s == ',') s++;

    /* flag field starts here, check if exactly "hot" */
    int is_hot = (s[0] == 'h' && s[1] == 'o' && s[2] == 't' && s[3] == '\0');

    /* compute value */
    uint64_t v = (uint64_t)(b + c + 28);
    if (is_hot) v += 29ULL;

    /* hash mixing */
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
