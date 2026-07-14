#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left;
    const uint64_t* r = right;
    size_t n = word_count;

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    while (n >= 8) {
      uint64_t a0 = l[0] & r[0];
      uint64_t a1 = l[1] & r[1];
      uint64_t a2 = l[2] & r[2];
      uint64_t a3 = l[3] & r[3];
      uint64_t a4 = l[4] & r[4];
      uint64_t a5 = l[5] & r[5];
      uint64_t a6 = l[6] & r[6];
      uint64_t a7 = l[7] & r[7];

      s0 += (uint64_t)__builtin_popcountll(a0);
      s1 += (uint64_t)__builtin_popcountll(a1);
      s2 += (uint64_t)__builtin_popcountll(a2);
      s3 += (uint64_t)__builtin_popcountll(a3);
      s0 += (uint64_t)__builtin_popcountll(a4);
      s1 += (uint64_t)__builtin_popcountll(a5);
      s2 += (uint64_t)__builtin_popcountll(a6);
      s3 += (uint64_t)__builtin_popcountll(a7);

      l += 8;
      r += 8;
      n -= 8;
    }

    while (n--) {
      uint64_t a = *l++ & *r++;
      s0 += (uint64_t)__builtin_popcountll(a);
    }

    total = s0 + s1 + s2 + s3;
  }
  return total;
}
