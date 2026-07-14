#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* ptr = rows[i];
    ptr += 2; // Skip 'r,'

    int id = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      id = id * 10 + (*ptr++ - '0');
    }
    ptr++; // Skip comma

    int a = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      a = a * 10 + (*ptr++ - '0');
    }
    ptr++;

    int b = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      b = b * 10 + (*ptr++ - '0');
    }
    ptr++;

    int c = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      c = c * 10 + (*ptr++ - '0');
    }
    ptr++;

    char flag[8] = {0};
    int j = 0;
    while (j < 7 && *ptr != '\0' && *ptr != ',') {
      flag[j++] = *ptr++;
    }

    uint64_t v = (uint64_t)(b + c + 11);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}