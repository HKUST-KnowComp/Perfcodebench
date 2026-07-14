#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  // Preserve baseline behavior when iters == 0
  uint64_t total = 0;
  const int64_t addConst = 0x9E3779B1LL; // constant added to each prefix value

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t acc = 0;

    const int32_t* p = deltas;
    size_t n = count;

    // Unroll by 8 for better ILP and reduced loop overhead
    size_t blocks = n / 8;
    while (blocks--) {
      current += (int64_t)p[0]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[1]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[2]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[3]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[4]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[5]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[6]; acc += (uint64_t)(current + addConst);
      current += (int64_t)p[7]; acc += (uint64_t)(current + addConst);
      p += 8;
    }

    // Handle remaining elements
    size_t rem = n & 7u;
    while (rem--) {
      current += (int64_t)(*p++);
      acc += (uint64_t)(current + addConst);
    }

    // Match baseline behavior: keep only the last iteration's total
    total = acc;
  }

  return total;
}
