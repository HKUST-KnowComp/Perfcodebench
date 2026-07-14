#include "interface.h"

#include <vector>

static inline uint64_t popcnt64_u64(uint64_t x) {
#if defined(__GNUG__) || defined(__clang__)
  return static_cast<uint64_t>(__builtin_popcountll(x));
#else
  // Portable fallback (Hacker's Delight popcount)
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return static_cast<uint64_t>((x * 0x0101010101010101ULL) >> 56);
#endif
}

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  // Match baseline semantics: if iters <= 0, return 0; otherwise, the popcount.
  if (iters <= 0) return 0;

  const uint64_t* p = words.data();
  std::size_t n = words.size();

  // Use multiple accumulators to reduce dependency chains and unroll for ILP.
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;

  for (; i + 15 < n; i += 16) {
    s0 += popcnt64_u64(p[i + 0]);
    s1 += popcnt64_u64(p[i + 1]);
    s2 += popcnt64_u64(p[i + 2]);
    s3 += popcnt64_u64(p[i + 3]);
    s0 += popcnt64_u64(p[i + 4]);
    s1 += popcnt64_u64(p[i + 5]);
    s2 += popcnt64_u64(p[i + 6]);
    s3 += popcnt64_u64(p[i + 7]);
    s0 += popcnt64_u64(p[i + 8]);
    s1 += popcnt64_u64(p[i + 9]);
    s2 += popcnt64_u64(p[i + 10]);
    s3 += popcnt64_u64(p[i + 11]);
    s0 += popcnt64_u64(p[i + 12]);
    s1 += popcnt64_u64(p[i + 13]);
    s2 += popcnt64_u64(p[i + 14]);
    s3 += popcnt64_u64(p[i + 15]);
  }

  for (; i + 3 < n; i += 4) {
    s0 += popcnt64_u64(p[i + 0]);
    s1 += popcnt64_u64(p[i + 1]);
    s2 += popcnt64_u64(p[i + 2]);
    s3 += popcnt64_u64(p[i + 3]);
  }

  uint64_t sum = s0 + s1 + s2 + s3;
  for (; i < n; ++i) {
    sum += popcnt64_u64(p[i]);
  }

  return sum;
}
