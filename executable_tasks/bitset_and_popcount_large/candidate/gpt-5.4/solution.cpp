#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t popcount64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return static_cast<uint64_t>(__builtin_popcountll(x));
#else
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
#endif
}

}  // namespace

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  if (n == 0 || iters <= 0) return 0;

  const uint64_t* l = left.data();
  const uint64_t* r = right.data();
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum0 = 0;
    uint64_t sum1 = 0;
    uint64_t sum2 = 0;
    uint64_t sum3 = 0;

    std::size_t i = 0;
    const std::size_t unrolled_end = n & ~static_cast<std::size_t>(3);
    for (; i < unrolled_end; i += 4) {
      sum0 += popcount64(l[i] & r[i]);
      sum1 += popcount64(l[i + 1] & r[i + 1]);
      sum2 += popcount64(l[i + 2] & r[i + 2]);
      sum3 += popcount64(l[i + 3] & r[i + 3]);
    }

    uint64_t total = sum0 + sum1 + sum2 + sum3;
    for (; i < n; ++i) {
      total += popcount64(l[i] & r[i]);
    }
    count = total;
  }

  return count;
}
