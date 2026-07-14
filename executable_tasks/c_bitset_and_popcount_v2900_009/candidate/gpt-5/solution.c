#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;

    // Unroll for better throughput
    for (; i + 7 < word_count; i += 8) {
      uint64_t w0 = left[i + 0] & right[i + 0];
      uint64_t w1 = left[i + 1] & right[i + 1];
      uint64_t w2 = left[i + 2] & right[i + 2];
      uint64_t w3 = left[i + 3] & right[i + 3];
      uint64_t w4 = left[i + 4] & right[i + 4];
      uint64_t w5 = left[i + 5] & right[i + 5];
      uint64_t w6 = left[i + 6] & right[i + 6];
      uint64_t w7 = left[i + 7] & right[i + 7];

      sum += (uint64_t)__builtin_popcountll((unsigned long long)w0);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w1);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w2);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w3);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w4);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w5);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w6);
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w7);
    }

    for (; i < word_count; ++i) {
      uint64_t w = left[i] & right[i];
      sum += (uint64_t)__builtin_popcountll((unsigned long long)w);
    }

    total = sum; // match baseline semantics: result of last iteration
  }

  return total;
}
