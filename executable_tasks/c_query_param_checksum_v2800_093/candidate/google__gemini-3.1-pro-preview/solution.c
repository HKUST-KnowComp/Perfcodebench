#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int parse_int_from(const char *s) {
    while (*s == ' ' || (unsigned char)(*s - '\t') <= 4) s++;
    int sign = 1;
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    int res = 0;
    while ((unsigned char)(*s - '0') <= 9) {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res * sign;
}

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    
    // Find first token (mimicking strtok skipping leading delimiters)
    while (*p == '&') p++;
    const char* p0 = p;
    
    // Find second token
    while (*p && *p != '&') p++;
    while (*p == '&') p++;
    const char* p1 = p;
    
    // Find third token
    while (*p && *p != '&') p++;
    while (*p == '&') p++;
    const char* p2 = p;
    
    int u = parse_int_from(p0 + 2);
    int s_val = parse_int_from(p1 + 6);
    int f = parse_int_from(p2 + 5);
    
    uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}
