#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline uint64_t popcount_u64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return (uint64_t)__builtin_popcountll((unsigned long long)x);
#else
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left;
    const uint64_t* r = right;
    size_t n = word_count;
    uint64_t sum = 0;

    while (n >= 8) {
      sum += popcount_u64(l[0] & r[0]);
      sum += popcount_u64(l[1] & r[1]);
      sum += popcount_u64(l[2] & r[2]);
      sum += popcount_u64(l[3] & r[3]);
      sum += popcount_u64(l[4] & r[4]);
      sum += popcount_u64(l[5] & r[5]);
      sum += popcount_u64(l[6] & r[6]);
      sum += popcount_u64(l[7] & r[7]);
      l += 8;
      r += 8;
      n -= 8;
    }

    while (n--) {
      sum += popcount_u64(*l++ & *r++);
    }

    total = sum;
  }

  return total;
}
