#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  const uint64_t* restrict l = left;
  const uint64_t* restrict r = right;

  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;
    size_t n = word_count;

    for (; i + 8 <= n; i += 8) {
      uint64_t a0 = l[i + 0] & r[i + 0];
      uint64_t a1 = l[i + 1] & r[i + 1];
      uint64_t a2 = l[i + 2] & r[i + 2];
      uint64_t a3 = l[i + 3] & r[i + 3];
      uint64_t a4 = l[i + 4] & r[i + 4];
      uint64_t a5 = l[i + 5] & r[i + 5];
      uint64_t a6 = l[i + 6] & r[i + 6];
      uint64_t a7 = l[i + 7] & r[i + 7];

      sum += (uint64_t)__builtin_popcountll(a0)
           + (uint64_t)__builtin_popcountll(a1)
           + (uint64_t)__builtin_popcountll(a2)
           + (uint64_t)__builtin_popcountll(a3)
           + (uint64_t)__builtin_popcountll(a4)
           + (uint64_t)__builtin_popcountll(a5)
           + (uint64_t)__builtin_popcountll(a6)
           + (uint64_t)__builtin_popcountll(a7);
    }

    for (; i < n; ++i) {
      sum += (uint64_t)__builtin_popcountll(l[i] & r[i]);
    }

    total = sum; // return last iteration's result, matching baseline semantics
  }

  return total;
}
