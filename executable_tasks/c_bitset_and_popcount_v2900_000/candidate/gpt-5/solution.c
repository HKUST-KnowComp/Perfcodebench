#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left;
    const uint64_t* r = right;
    size_t n = word_count;
    uint64_t sum = 0;

    // Process in chunks of 8 for better instruction-level parallelism
    while (n >= 8) {
      uint64_t w0 = l[0] & r[0];
      uint64_t w1 = l[1] & r[1];
      uint64_t w2 = l[2] & r[2];
      uint64_t w3 = l[3] & r[3];
      uint64_t w4 = l[4] & r[4];
      uint64_t w5 = l[5] & r[5];
      uint64_t w6 = l[6] & r[6];
      uint64_t w7 = l[7] & r[7];

      sum += (uint64_t)__builtin_popcountll(w0);
      sum += (uint64_t)__builtin_popcountll(w1);
      sum += (uint64_t)__builtin_popcountll(w2);
      sum += (uint64_t)__builtin_popcountll(w3);
      sum += (uint64_t)__builtin_popcountll(w4);
      sum += (uint64_t)__builtin_popcountll(w5);
      sum += (uint64_t)__builtin_popcountll(w6);
      sum += (uint64_t)__builtin_popcountll(w7);

      l += 8;
      r += 8;
      n -= 8;
    }

    // Handle remaining words
    while (n--) {
      uint64_t w = (*l++ & *r++);
      sum += (uint64_t)__builtin_popcountll(w);
    }

    // Match baseline behavior: return the last iteration's result
    total = sum;
  }
  return total;
}
