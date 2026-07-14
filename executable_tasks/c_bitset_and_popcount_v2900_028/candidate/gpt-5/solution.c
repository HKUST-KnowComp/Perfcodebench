#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline uint64_t popcnt64(uint64_t x) {
  return (uint64_t)__builtin_popcountll(x);
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  const uint64_t* restrict l = left;
  const uint64_t* restrict r = right;

  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* lp = l;
    const uint64_t* rp = r;
    size_t n = word_count;

    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;

    while (n >= 16) {
      sum0 += popcnt64(lp[0]  & rp[0]);
      sum1 += popcnt64(lp[1]  & rp[1]);
      sum2 += popcnt64(lp[2]  & rp[2]);
      sum3 += popcnt64(lp[3]  & rp[3]);

      sum0 += popcnt64(lp[4]  & rp[4]);
      sum1 += popcnt64(lp[5]  & rp[5]);
      sum2 += popcnt64(lp[6]  & rp[6]);
      sum3 += popcnt64(lp[7]  & rp[7]);

      sum0 += popcnt64(lp[8]  & rp[8]);
      sum1 += popcnt64(lp[9]  & rp[9]);
      sum2 += popcnt64(lp[10] & rp[10]);
      sum3 += popcnt64(lp[11] & rp[11]);

      sum0 += popcnt64(lp[12] & rp[12]);
      sum1 += popcnt64(lp[13] & rp[13]);
      sum2 += popcnt64(lp[14] & rp[14]);
      sum3 += popcnt64(lp[15] & rp[15]);

      lp += 16; rp += 16; n -= 16;
    }

    while (n >= 4) {
      sum0 += popcnt64(lp[0] & rp[0]);
      sum1 += popcnt64(lp[1] & rp[1]);
      sum2 += popcnt64(lp[2] & rp[2]);
      sum3 += popcnt64(lp[3] & rp[3]);
      lp += 4; rp += 4; n -= 4;
    }

    uint64_t tail = 0;
    while (n--) {
      tail += popcnt64(*lp++ & *rp++);
    }

    result = (sum0 + sum1) + (sum2 + sum3) + tail;
  }

  return result;
}
