#include "interface.h"

static inline uint64_t popcount64_u64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return static_cast<uint64_t>(__builtin_popcountll(x));
#elif defined(_MSC_VER)
  return static_cast<uint64_t>(__popcnt64(x));
#else
  // Fallback: parallel bit count for 64-bit
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  x = x + (x >> 8);
  x = x + (x >> 16);
  x = x + (x >> 32);
  return x & 0x7FULL;
#endif
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  uint64_t count = 0;
  const uint64_t* __restrict lp = left.data();
  const uint64_t* __restrict rp = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t total = 0;
    std::size_t i = 0;

    const std::size_t unroll = n & ~static_cast<std::size_t>(3);
    for (; i < unroll; i += 4) {
      total += popcount64_u64(lp[i] & rp[i]);
      total += popcount64_u64(lp[i + 1] & rp[i + 1]);
      total += popcount64_u64(lp[i + 2] & rp[i + 2]);
      total += popcount64_u64(lp[i + 3] & rp[i + 3]);
    }
    for (; i < n; ++i) {
      total += popcount64_u64(lp[i] & rp[i]);
    }

    count = total; // return value is from the last iteration
  }

  return count;
}
