#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    size_t len = strlen(row);
    size_t first_amp = 0;
    while (first_amp < len && row[first_amp] != '&') {
      first_amp++;
    }
    size_t second_amp = first_amp + 1;
    while (second_amp < len && row[second_amp] != '&') {
      second_amp++;
    }

    const char* u_start = row + 2;
    const char* u_end = row + first_amp;
    const char* s_start = row + (first_amp + 1) + 6;
    const char* s_end = row + second_amp;
    const char* f_start = row + (second_amp + 1) + 5;
    const char* f_end = row + len;

    int u = 0;
    while (u_start < u_end) {
      if (*u_start >= '0' && *u_start <= '9') {
        u = u * 10 + (*u_start - '0');
      }
      u_start++;
    }

    int s = 0;
    while (s_start < s_end) {
      if (*s_start >= '0' && *s_start <= '9') {
        s = s * 10 + (*s_start - '0');
      }
      s_start++;
    }

    int f = 0;
    while (f_start < f_end) {
      if (*f_start >= '0' && *f_start <= '9') {
        f = f * 10 + (*f_start - '0');
      }
      f_start++;
    }

    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}