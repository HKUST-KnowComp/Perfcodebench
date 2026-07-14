#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    row++; // Skip 'r'
    char* end;
    int id = (int)strtol(row, &end, 10);
    row = end + 1; // Skip comma
    int a = (int)strtol(row, &end, 10);
    row = end + 1;
    int b = (int)strtol(row, &end, 10);
    row = end + 1;
    int c = (int)strtol(row, &end, 10);
    row = end + 1;
    char flag[8] = {0};
    int j = 0;
    while (j < 7 && row[j] != ',' && row[j] != '\0') {
      flag[j] = row[j];
      j++;
    }
    flag[j] = '\0';
    uint64_t v = (uint64_t)(b + c + 10);
    if (strcmp(flag, "hot") == 0) v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}