#include "interface.h"

static inline unsigned popcount64_u64(uint64_t x) {
#if defined(__clang__) || defined(__GNUC__)
  return static_cast<unsigned>(__builtin_popcountll(x));
#else
  // Portable SWAR popcount fallback
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return static_cast<unsigned>((x * 0x0101010101010101ULL) >> 56);
#endif
}

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int /*iters*/) {
  const uint64_t* p = words.data();
  std::size_t n = words.size();

  uint64_t sum0 = 0, sum1 = 0;
  std::size_t i = 0;

  // Unroll by 8 for better ILP
  for (; i + 8 <= n; i += 8) {
    sum0 += popcount64_u64(p[i + 0]);
    sum0 += popcount64_u64(p[i + 1]);
    sum0 += popcount64_u64(p[i + 2]);
    sum0 += popcount64_u64(p[i + 3]);
    sum1 += popcount64_u64(p[i + 4]);
    sum1 += popcount64_u64(p[i + 5]);
    sum1 += popcount64_u64(p[i + 6]);
    sum1 += popcount64_u64(p[i + 7]);
  }

  for (; i < n; ++i) {
    sum0 += popcount64_u64(p[i]);
  }

  return sum0 + sum1;
}
