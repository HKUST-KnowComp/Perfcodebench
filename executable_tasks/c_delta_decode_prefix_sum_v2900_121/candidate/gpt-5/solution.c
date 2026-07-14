#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const uint64_t k = 0x9E3779B1ULL;
  const uint64_t k_times_count = k * (uint64_t)count;
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum_current = 0;

    const int32_t* p = deltas;
    size_t n = count;

    size_t n4 = n >> 2; // n / 4
    while (n4--) {
      current += (int64_t)*p++; sum_current += (uint64_t)current;
      current += (int64_t)*p++; sum_current += (uint64_t)current;
      current += (int64_t)*p++; sum_current += (uint64_t)current;
      current += (int64_t)*p++; sum_current += (uint64_t)current;
    }

    switch (n & 3u) {
      case 3:
        current += (int64_t)*p++; sum_current += (uint64_t)current;
        // fall through
      case 2:
        current += (int64_t)*p++; sum_current += (uint64_t)current;
        // fall through
      case 1:
        current += (int64_t)*p++; sum_current += (uint64_t)current;
        // fall through
      default:
        break;
    }

    total = sum_current + k_times_count;
  }

  return total;
}
