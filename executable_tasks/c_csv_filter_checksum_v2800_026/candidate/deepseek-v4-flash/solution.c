#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    // skip 'r' and id
    p++;
    while (*p != ',') p++;
    p++;
    // skip field a
    while (*p != ',') p++;
    p++;
    // parse b
    int b = 0;
    while (*p != ',') {
      b = b * 10 + (*p - '0');
      p++;
    }
    p++;
    // parse c
    int c = 0;
    while (*p != ',') {
      c = c * 10 + (*p - '0');
      p++;
    }
    p++;
    // read flag
    char flag[8] = {0};
    int fi = 0;
    while (*p && *p != ',' && *p != '\n' && *p != '\r' && fi < 7) {
      flag[fi++] = *p++;
    }
    int hot = (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0');
    uint64_t v = (uint64_t)(b + c + 10);
    if (hot) v += 29;
    hash ^= (v + 1);
    hash *= 1099511628211ULL;
  }
  return hash;
}