#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t K = 0x9E3779B1LL;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    const int32_t* p = deltas;
    size_t n = count;

    size_t chunks = n / 4;
    while (chunks--) {
      int64_t c;

      c = current + (int64_t)p[0];
      sum += (uint64_t)(c + K);
      current = c;

      c = current + (int64_t)p[1];
      sum += (uint64_t)(c + K);
      current = c;

      c = current + (int64_t)p[2];
      sum += (uint64_t)(c + K);
      current = c;

      c = current + (int64_t)p[3];
      sum += (uint64_t)(c + K);
      current = c;

      p += 4;
    }

    n &= 3u;
    while (n--) {
      current += (int64_t)(*p++);
      sum += (uint64_t)(current + K);
    }

    total = sum; // Return the result of the last iteration
  }

  return total;
}
