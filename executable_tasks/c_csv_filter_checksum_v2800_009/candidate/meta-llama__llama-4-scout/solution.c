#include "interface.h"

#include <stdint.h>
#include <string.h>

#define CSV_FIELD_MAX_LENGTH 8

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    int id, a, b, c;
    char flag[CSV_FIELD_MAX_LENGTH];
    const char* p = row;
    // parse id
    while (*p != ',') p++;
    p++;
    // parse a
    while (*p != ',') p++;
    p++;
    // parse b
    const char* b_start = p;
    while (*p != ',') p++;
    int b_len = p - b_start;
    int b_val = 0;
    for (int j = 0; j < b_len; j++) {
      b_val = b_val * 10 + (b_start[j] - '0');
    }
    p++;
    // parse c
    const char* c_start = p;
    while (*p != ',') p++;
    int c_len = p - c_start;
    int c_val = 0;
    for (int j = 0; j < c_len; j++) {
      c_val = c_val * 10 + (c_start[j] - '0');
    }
    p++;
    // parse flag
    const char* flag_start = p;
    int flag_len = 0;
    while (*p != '\0' && flag_len < CSV_FIELD_MAX_LENGTH - 1) {
      flag[flag_len++] = *p++;
    }
    flag[flag_len] = '\0';
    uint64_t v = (uint64_t)(b_val + c_val + 16);
    if (strncmp(flag, "hot", 3) == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}