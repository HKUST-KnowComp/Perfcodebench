#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t K = 0x9E3779B1LL;
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const int32_t* __restrict p = deltas;
    size_t n = count;
    uint64_t sum = 0;
    int64_t current = 0;

    size_t blocks = n / 8;
    while (blocks--) {
      int64_t c = current;
      c += (int64_t)p[0]; sum += (uint64_t)(c + K);
      c += (int64_t)p[1]; sum += (uint64_t)(c + K);
      c += (int64_t)p[2]; sum += (uint64_t)(c + K);
      c += (int64_t)p[3]; sum += (uint64_t)(c + K);
      c += (int64_t)p[4]; sum += (uint64_t)(c + K);
      c += (int64_t)p[5]; sum += (uint64_t)(c + K);
      c += (int64_t)p[6]; sum += (uint64_t)(c + K);
      c += (int64_t)p[7]; sum += (uint64_t)(c + K);
      p += 8;
      current = c;
    }

    switch (n & 7u) {
      case 7: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      case 6: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      case 5: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      case 4: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      case 3: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      case 2: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      case 1: current += (int64_t)p[0]; sum += (uint64_t)(current + K); ++p;
      default: break;
    }

    // Match baseline behavior: result from the last iteration only
    total = sum;
  }

  return total;
}
