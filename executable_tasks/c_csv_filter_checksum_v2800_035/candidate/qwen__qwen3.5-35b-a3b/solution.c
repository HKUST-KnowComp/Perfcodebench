#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* ptr = rows[i];
    if (*ptr != 'r') continue;
    ptr++;
    
    int id = 0;
    int a = 0, b = 0, c = 0;
    
    // Parse id
    while (*ptr >= '0' && *ptr <= '9') {
      id = id * 10 + (*ptr++ - '0');
    }
    if (*ptr == ',') ptr++;
    
    // Parse a
    while (*ptr >= '0' && *ptr <= '9') {
      a = a * 10 + (*ptr++ - '0');
    }
    if (*ptr == ',') ptr++;
    
    // Parse b
    while (*ptr >= '0' && *ptr <= '9') {
      b = b * 10 + (*ptr++ - '0');
    }
    if (*ptr == ',') ptr++;
    
    // Parse c
    while (*ptr >= '0' && *ptr <= '9') {
      c = c * 10 + (*ptr++ - '0');
    }
    if (*ptr == ',') ptr++;
    
    // Parse flag (up to 7 chars)
    char flag[8] = {0};
    int flag_idx = 0;
    while (flag_idx < 7 && *ptr != ',' && *ptr != '\n' && *ptr != '\0') {
      flag[flag_idx++] = *ptr++;
    }
    flag[flag_idx] = '\0';
    
    uint64_t v = (uint64_t)(b + c + 19);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
      v += 29ULL;
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}