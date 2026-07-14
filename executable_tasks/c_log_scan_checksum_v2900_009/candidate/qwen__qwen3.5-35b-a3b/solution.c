#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* row = rows[i];
    size_t len = strlen(row);
    uint64_t v = 0;
    if (len == 15) {
      if (row[6] == '-') {
        int valid_digits = 1;
        for (int j = 2; j <= 5; ++j) {
          if (row[j] < '0' || row[j] > '9') {
            valid_digits = 0;
            break;
          }
        }
        if (valid_digits) {
          int valid_hex = 1;
          for (int j = 7; j <= 14; ++j) {
            unsigned char c = row[j];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
              valid_hex = 0;
              break;
            }
          }
          if (valid_hex) {
            v = (unsigned char)row[2] + (unsigned char)row[7];
          }
        }
      }
    }
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}