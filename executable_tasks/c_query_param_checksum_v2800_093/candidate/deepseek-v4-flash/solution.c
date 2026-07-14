#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // Find first '&'
    const char* p1 = p;
    while (*p1 && *p1 != '&') ++p1;
    const char* token1_start = p;
    const char* token2_start = (*p1 == '&') ? p1 + 1 : p1;
    // Find second '&'
    const char* p2 = token2_start;
    while (*p2 && *p2 != '&') ++p2;
    const char* token3_start = (*p2 == '&') ? p2 + 1 : p2;
    // Parse u from token1_start + 2 (skip "u=")
    const char* u_ptr = token1_start + 2;
    int u = 0;
    while (*u_ptr >= '0' && *u_ptr <= '9') {
      u = u * 10 + (*u_ptr - '0');
      ++u_ptr;
    }
    // Parse s from token2_start + 6 (skip "state=")
    const char* s_ptr = token2_start + 6;
    int s = 0;
    while (*s_ptr >= '0' && *s_ptr <= '9') {
      s = s * 10 + (*s_ptr - '0');
      ++s_ptr;
    }
    // Parse f from token3_start + 5 (skip "flag=")
    const char* f_ptr = token3_start + 5;
    int f = 0;
    while (*f_ptr >= '0' && *f_ptr <= '9') {
      f = f * 10 + (*f_ptr - '0');
      ++f_ptr;
    }
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}