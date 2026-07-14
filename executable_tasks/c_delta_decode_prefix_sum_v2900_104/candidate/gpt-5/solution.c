#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t addC = (uint64_t)(int64_t)0x9E3779B1LL;
  const uint64_t constTerm = addC * (uint64_t)count;

  for (int iter = 0; iter < iters; ++iter) {
    const int32_t* p = deltas;
    const int32_t* end = deltas + count;
    const int32_t* end8 = deltas + (count & ~(size_t)7);

    int64_t current = 0;
    uint64_t sumPrefixes = 0;

    // Unrolled loop processing 8 elements per iteration
    while (p < end8) {
      current += (int64_t)p[0]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[1]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[2]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[3]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[4]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[5]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[6]; sumPrefixes += (uint64_t)current;
      current += (int64_t)p[7]; sumPrefixes += (uint64_t)current;
      p += 8;
    }

    // Handle remaining elements
    while (p < end) {
      current += (int64_t)*p++;
      sumPrefixes += (uint64_t)current;
    }

    // Add the constant offset contribution once
    total = sumPrefixes + constTerm;
  }

  return total;
}
