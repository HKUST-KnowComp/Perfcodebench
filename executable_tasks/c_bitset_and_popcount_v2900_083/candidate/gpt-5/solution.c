#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left;
    const uint64_t* r = right;
    size_t n = word_count;

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    // Unroll by 8 for throughput
    size_t blocks = n / 8;
    for (size_t b = 0; b < blocks; ++b) {
      uint64_t w0 = l[0] & r[0];
      uint64_t w1 = l[1] & r[1];
      uint64_t w2 = l[2] & r[2];
      uint64_t w3 = l[3] & r[3];
      uint64_t w4 = l[4] & r[4];
      uint64_t w5 = l[5] & r[5];
      uint64_t w6 = l[6] & r[6];
      uint64_t w7 = l[7] & r[7];

      s0 += (uint64_t)__builtin_popcountll(w0);
      s1 += (uint64_t)__builtin_popcountll(w1);
      s2 += (uint64_t)__builtin_popcountll(w2);
      s3 += (uint64_t)__builtin_popcountll(w3);
      s0 += (uint64_t)__builtin_popcountll(w4);
      s1 += (uint64_t)__builtin_popcountll(w5);
      s2 += (uint64_t)__builtin_popcountll(w6);
      s3 += (uint64_t)__builtin_popcountll(w7);

      l += 8;
      r += 8;
    }

    // Remainder
    size_t rem = n & 7u;
    for (size_t i = 0; i < rem; ++i) {
      s0 += (uint64_t)__builtin_popcountll(l[i] & r[i]);
    }

    result = s0 + s1 + s2 + s3;
  }
  return result;
}
