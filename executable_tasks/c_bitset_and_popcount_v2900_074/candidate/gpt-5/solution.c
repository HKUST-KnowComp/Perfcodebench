#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  if (word_count == 0 || iters <= 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* restrict l = left;
    const uint64_t* restrict r = right;
    size_t n = word_count;
    size_t i = 0;

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    // Unroll by 8 for throughput and ILP
    for (; i + 7 < n; i += 8) {
      uint64_t w0 = l[i]     & r[i];
      uint64_t w1 = l[i + 1] & r[i + 1];
      uint64_t w2 = l[i + 2] & r[i + 2];
      uint64_t w3 = l[i + 3] & r[i + 3];
      uint64_t w4 = l[i + 4] & r[i + 4];
      uint64_t w5 = l[i + 5] & r[i + 5];
      uint64_t w6 = l[i + 6] & r[i + 6];
      uint64_t w7 = l[i + 7] & r[i + 7];

      s0 += (uint64_t)__builtin_popcountll(w0);
      s1 += (uint64_t)__builtin_popcountll(w1);
      s2 += (uint64_t)__builtin_popcountll(w2);
      s3 += (uint64_t)__builtin_popcountll(w3);
      s0 += (uint64_t)__builtin_popcountll(w4);
      s1 += (uint64_t)__builtin_popcountll(w5);
      s2 += (uint64_t)__builtin_popcountll(w6);
      s3 += (uint64_t)__builtin_popcountll(w7);
    }

    // Unroll by 4 for remaining chunks
    for (; i + 3 < n; i += 4) {
      uint64_t w0 = l[i]     & r[i];
      uint64_t w1 = l[i + 1] & r[i + 1];
      uint64_t w2 = l[i + 2] & r[i + 2];
      uint64_t w3 = l[i + 3] & r[i + 3];

      s0 += (uint64_t)__builtin_popcountll(w0);
      s1 += (uint64_t)__builtin_popcountll(w1);
      s2 += (uint64_t)__builtin_popcountll(w2);
      s3 += (uint64_t)__builtin_popcountll(w3);
    }

    uint64_t sum = s0 + s1 + s2 + s3;

    // Tail
    for (; i < n; ++i) {
      sum += (uint64_t)__builtin_popcountll(l[i] & r[i]);
    }

    total = sum; // match baseline: return last iteration's result
  }

  return total;
}
