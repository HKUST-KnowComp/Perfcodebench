#include "interface.h"

#include <vector>

static inline uint64_t popcnt64(uint64_t x) {
#if defined(__clang__) || defined(__GNUC__)
  return static_cast<uint64_t>(__builtin_popcountll(x));
#else
  // Portable fallback (Hacker's Delight)
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  const uint64_t* p = words.data();
  const std::size_t n = words.size();
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    std::size_t i = 0;

    // Unrolled loop for better throughput
    for (; i + 8 <= n; i += 8) {
      sum += popcnt64(p[i + 0]);
      sum += popcnt64(p[i + 1]);
      sum += popcnt64(p[i + 2]);
      sum += popcnt64(p[i + 3]);
      sum += popcnt64(p[i + 4]);
      sum += popcnt64(p[i + 5]);
      sum += popcnt64(p[i + 6]);
      sum += popcnt64(p[i + 7]);
    }
    for (; i < n; ++i) {
      sum += popcnt64(p[i]);
    }

    count = sum;
  }

  return count;
}
