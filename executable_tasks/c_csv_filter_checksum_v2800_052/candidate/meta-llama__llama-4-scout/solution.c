#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    size_t id_len = strcspn(row, ",");
    row += id_len + 1;
    size_t a_len = strcspn(row, ",");
    row += a_len + 1;
    size_t b_len = strcspn(row, ",");
    int b = atoi(row);
    row += b_len + 1;
    int c = atoi(row);
    row += strcspn(row, ",");
    size_t flag_len = strcspn(row, "\n");
    char flag[8];
    strncpy(flag, row, flag_len > 7 ? 7 : flag_len);
    flag[flag_len > 7 ? 7 : flag_len] = '\0';

    uint64_t v = (uint64_t)(b + c + 13);
    if (strncmp(flag, "hot", 3) == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}