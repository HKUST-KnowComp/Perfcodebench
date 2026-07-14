#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t C = (int64_t)0x9E3779B1u;
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t t = 0;

    const int32_t* p = deltas;
    const int32_t* end = deltas + count;

    // Unrolled by 4 for better ILP and reduced loop overhead
    for (; p + 4 <= end; p += 4) {
      int64_t c0 = current + (int64_t)p[0];
      int64_t c1 = c0      + (int64_t)p[1];
      int64_t c2 = c1      + (int64_t)p[2];
      int64_t c3 = c2      + (int64_t)p[3];

      t += (uint64_t)(c0 + C);
      t += (uint64_t)(c1 + C);
      t += (uint64_t)(c2 + C);
      t += (uint64_t)(c3 + C);

      current = c3;
    }

    // Handle remaining elements
    for (; p < end; ++p) {
      current += (int64_t)(*p);
      t += (uint64_t)(current + C);
    }

    // Mirror baseline semantics: result is from the last iteration
    total = t;
  }

  return total;
}
