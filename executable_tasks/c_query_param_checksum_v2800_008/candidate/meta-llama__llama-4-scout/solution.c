#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    const char* p = rows[i];
    int u, s, f;
    // manual parsing
    p += 2; while (*p != '&') p++; p++; // skip 'u=' and move to 's='
    u = atoi(p); p += strlen(p) + 1; // consume 'u=<num>&'
    p += 6; while (*p != '&') p++; p++; // skip 's=' and move to 'f='
    s = atoi(p); p += strlen(p) + 1; // consume 's=<num>&'
    p += 5; 
    f = atoi(p);
    uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}