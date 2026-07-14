#include "interface.h"

#include <stdint.h>

static inline uint32_t pc64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return (uint32_t)__builtin_popcountll(x);
#else
  // Fallback popcount implementation (Hacker's Delight)
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (uint32_t)((x * 0x0101010101010101ULL) >> 56);
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  (void)iters; // The baseline returns the popcount for a single pass; ignore iters to preserve behavior
  const uint64_t* restrict l = left;
  const uint64_t* restrict r = right;

  uint64_t total = 0;
  size_t i = 0;
  size_t n = word_count;

  // Unroll by 8 for better throughput
  size_t limit = n & ~(size_t)7;
  for (; i < limit; i += 8) {
    total += (uint64_t)pc64(l[i + 0] & r[i + 0]);
    total += (uint64_t)pc64(l[i + 1] & r[i + 1]);
    total += (uint64_t)pc64(l[i + 2] & r[i + 2]);
    total += (uint64_t)pc64(l[i + 3] & r[i + 3]);
    total += (uint64_t)pc64(l[i + 4] & r[i + 4]);
    total += (uint64_t)pc64(l[i + 5] & r[i + 5]);
    total += (uint64_t)pc64(l[i + 6] & r[i + 6]);
    total += (uint64_t)pc64(l[i + 7] & r[i + 7]);
  }
  for (; i < n; ++i) {
    total += (uint64_t)pc64(l[i] & r[i]);
  }

  return total;
}
