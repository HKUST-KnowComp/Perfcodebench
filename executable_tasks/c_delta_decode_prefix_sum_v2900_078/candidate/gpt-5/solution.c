#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t C = 0x9E3779B1LL; // constant added per decoded value
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const int32_t* p = deltas;
    int64_t current = 0;
    uint64_t sum = 0;

    size_t blocks = count >> 3; // count / 8
    while (blocks--) {
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
    }

    size_t rem = count & 7; // count % 8
    while (rem--) {
      current += (int64_t)*p++;
      sum += (uint64_t)(current + C);
    }

    total = sum; // keep last iteration's result, matching baseline semantics
  }

  return total;
}
