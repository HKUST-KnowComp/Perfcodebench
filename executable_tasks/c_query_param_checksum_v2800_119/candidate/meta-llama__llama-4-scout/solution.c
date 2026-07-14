#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    int u = 0, s = 0, f = 0;
    int state = 0;
    for (const char* p = row; *p; ++p) {
      if (*p == '&') {
        state++;
        continue;
      }
      if (state == 0 && *p == '=' && *(p-1) == 'u') {
        const char* start = p + 1;
        int val = 0;
        while (isdigit(*(start++))) {
          val = val * 10 + (*(start-1) - '0');
        }
        u = val;
      } else if (state == 1 && *p == '=' && *(p-1) == 's') {
        const char* start = p + 1;
        int val = 0;
        while (isdigit(*(start++))) {
          val = val * 10 + (*(start-1) - '0');
        }
        s = val;
      } else if (state == 2 && *p == '=' && *(p-1) == 'f') {
        const char* start = p + 1;
        int val = 0;
        while (isdigit(*(start++))) {
          val = val * 10 + (*(start-1) - '0');
        }
        f = val;
      }
    }
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}