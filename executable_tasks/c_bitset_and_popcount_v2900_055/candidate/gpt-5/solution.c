#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline uint64_t popcnt64(uint64_t x) {
  return (uint64_t)__builtin_popcountll((unsigned long long)x);
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;
    size_t n = word_count;

    // Unroll by 4 for better ILP
    for (; i + 4 <= n; i += 4) {
      uint64_t w0 = left[i] & right[i];
      uint64_t w1 = left[i + 1] & right[i + 1];
      uint64_t w2 = left[i + 2] & right[i + 2];
      uint64_t w3 = left[i + 3] & right[i + 3];
      sum += popcnt64(w0);
      sum += popcnt64(w1);
      sum += popcnt64(w2);
      sum += popcnt64(w3);
    }
    for (; i < n; ++i) {
      sum += popcnt64(left[i] & right[i]);
    }

    // Match baseline semantics: return the last iteration's result
    total = sum;
  }
  return total;
}
