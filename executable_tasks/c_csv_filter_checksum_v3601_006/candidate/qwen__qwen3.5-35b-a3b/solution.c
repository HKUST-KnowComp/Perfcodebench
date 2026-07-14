#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* ptr = rows[i];
    // Skip 'r' and first comma
    if (*ptr == 'r') ptr++;
    if (*ptr == ',') ptr++;

    // Skip id (digits until comma)
    while (*ptr >= '0' && *ptr <= '9') ptr++;
    if (*ptr == ',') ptr++;

    // Skip a (digits until comma)
    while (*ptr >= '0' && *ptr <= '9') ptr++;
    if (*ptr == ',') ptr++;

    // Parse b
    int b = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      b = b * 10 + (*ptr - '0');
      ptr++;
    }
    if (*ptr == ',') ptr++;

    // Parse c
    int c = 0;
    while (*ptr >= '0' && *ptr <= '9') {
      c = c * 10 + (*ptr - '0');
      ptr++;
    }
    if (*ptr == ',') ptr++;

    // Read flag
    char flag[8];
    int i = 0;
    while (i < 7 && *ptr != ',' && *ptr != '\0') {
      flag[i++] = *ptr++;
    }
    flag[i] = '\0';

    uint64_t v = (uint64_t)(b + c + 25);
    if (strcmp(flag, "hot") == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}