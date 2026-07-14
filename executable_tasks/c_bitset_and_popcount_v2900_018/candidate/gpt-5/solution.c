#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint64_t popcnt64(uint64_t x) {
  return (uint64_t)__builtin_popcountll(x);
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  const uint64_t* restrict l = left;
  const uint64_t* restrict r = right;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;

    // Unroll by 8 with four independent accumulators to reduce dependency chain
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (; i + 7 < word_count; i += 8) {
      uint64_t a0 = l[i + 0] & r[i + 0];
      uint64_t a1 = l[i + 1] & r[i + 1];
      uint64_t a2 = l[i + 2] & r[i + 2];
      uint64_t a3 = l[i + 3] & r[i + 3];
      uint64_t a4 = l[i + 4] & r[i + 4];
      uint64_t a5 = l[i + 5] & r[i + 5];
      uint64_t a6 = l[i + 6] & r[i + 6];
      uint64_t a7 = l[i + 7] & r[i + 7];

      s0 += popcnt64(a0) + popcnt64(a4);
      s1 += popcnt64(a1) + popcnt64(a5);
      s2 += popcnt64(a2) + popcnt64(a6);
      s3 += popcnt64(a3) + popcnt64(a7);
    }
    sum += s0 + s1 + s2 + s3;

    // Handle remaining words
    for (; i < word_count; ++i) {
      sum += popcnt64(l[i] & r[i]);
    }

    total = sum; // per baseline semantics: keep last iteration's result
  }

  return total;
}
