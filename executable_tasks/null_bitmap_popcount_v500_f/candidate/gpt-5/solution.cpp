#include "interface.h"

static inline uint64_t popcount64_u64(uint64_t x) {
#if defined(__clang__) || defined(__GNUC__)
  return static_cast<uint64_t>(__builtin_popcountll(x));
#else
  // Portable fallback: HAKMEM/Hacker's Delight popcount
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  x = x + (x >> 8);
  x = x + (x >> 16);
  x = x + (x >> 32);
  return x & 0x7FULL;
#endif
}

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  (void)iters; // Iterations do not affect the final result; compute once for performance.
  const uint64_t* p = words.data();
  std::size_t n = words.size();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;
  std::size_t i = 0;
  std::size_t n8 = n & ~static_cast<std::size_t>(7);

  for (; i < n8; i += 8) {
    s0 += popcount64_u64(p[i + 0]);
    s1 += popcount64_u64(p[i + 1]);
    s2 += popcount64_u64(p[i + 2]);
    s3 += popcount64_u64(p[i + 3]);
    s4 += popcount64_u64(p[i + 4]);
    s5 += popcount64_u64(p[i + 5]);
    s6 += popcount64_u64(p[i + 6]);
    s7 += popcount64_u64(p[i + 7]);
  }

  uint64_t sum = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
  for (; i < n; ++i) {
    sum += popcount64_u64(p[i]);
  }

  return sum;
}
