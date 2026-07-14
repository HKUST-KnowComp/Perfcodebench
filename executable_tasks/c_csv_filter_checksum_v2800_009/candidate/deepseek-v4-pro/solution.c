#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* s = rows[i];
    if (*s == 'r') ++s;
    /* Skip id field */
    while (*s && *s != ',') ++s;
    if (*s == ',') ++s;
    /* Skip a field */
    while (*s && *s != ',') ++s;
    if (*s == ',') ++s;
    /* Parse b */
    int b = 0;
    while (*s >= '0' && *s <= '9') {
      b = b * 10 + (*s++ - '0');
    }
    if (*s == ',') ++s;
    /* Parse c */
    int c = 0;
    while (*s >= '0' && *s <= '9') {
      c = c * 10 + (*s++ - '0');
    }
    if (*s == ',') ++s;
    const char* flag = s;
    uint64_t v = (uint64_t)(b + c + 16);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0')
      v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}