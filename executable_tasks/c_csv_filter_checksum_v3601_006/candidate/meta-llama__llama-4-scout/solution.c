#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    const char* id_str = row;
    const char* a_str = strchr(row, ',') + 1;
    const char* b_str = strchr(a_str, ',') + 1;
    const char* c_str = strchr(b_str, ',') + 1;
    const char* flag_str = strchr(c_str, ',') + 1;
    size_t flag_len = strchr(flag_str, ',') ? strchr(flag_str, ',') - flag_str : strlen(flag_str);
    int id = atoi(id_str);
    int a = atoi(a_str);
    int b = atoi(b_str);
    int c = atoi(c_str);
    uint64_t v = (uint64_t)(b + c + 25);
    if (flag_len == 3 && memcmp(flag_str, "hot", 3) == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}