#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint32_t popcnt64_u64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return (uint32_t)__builtin_popcountll(x);
#else
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (uint32_t)((x * 0x0101010101010101ULL) >> 56);
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left;
    const uint64_t* r = right;

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    size_t n = word_count;
    size_t blocks = n >> 3; // process 8 words per block
    while (blocks--) {
      uint64_t a0 = l[0] & r[0];
      uint64_t a1 = l[1] & r[1];
      uint64_t a2 = l[2] & r[2];
      uint64_t a3 = l[3] & r[3];
      uint64_t a4 = l[4] & r[4];
      uint64_t a5 = l[5] & r[5];
      uint64_t a6 = l[6] & r[6];
      uint64_t a7 = l[7] & r[7];

      s0 += (uint64_t)popcnt64_u64(a0) + (uint64_t)popcnt64_u64(a4);
      s1 += (uint64_t)popcnt64_u64(a1) + (uint64_t)popcnt64_u64(a5);
      s2 += (uint64_t)popcnt64_u64(a2) + (uint64_t)popcnt64_u64(a6);
      s3 += (uint64_t)popcnt64_u64(a3) + (uint64_t)popcnt64_u64(a7);

      l += 8;
      r += 8;
    }

    uint64_t sum = s0 + s1 + s2 + s3;

    size_t rem = n & 7;
    for (size_t i = 0; i < rem; ++i) {
      sum += (uint64_t)popcnt64_u64(l[i] & r[i]);
    }

    total = sum; // same semantics as baseline: result from the last iteration
  }

  return total;
}
