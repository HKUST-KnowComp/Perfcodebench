#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t kConst = 0x9E3779B1LL;
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    const int32_t* p = deltas;
    const int32_t* end = deltas + count;

    // Process in chunks of 4 to reduce loop overhead
    while (p + 4 <= end) {
      current += (int64_t)p[0];
      sum += (uint64_t)(current + kConst);
      current += (int64_t)p[1];
      sum += (uint64_t)(current + kConst);
      current += (int64_t)p[2];
      sum += (uint64_t)(current + kConst);
      current += (int64_t)p[3];
      sum += (uint64_t)(current + kConst);
      p += 4;
    }
    while (p < end) {
      current += (int64_t)(*p++);
      sum += (uint64_t)(current + kConst);
    }

    total = sum; // per baseline semantics: return last iteration's sum
  }
  return total;
}
