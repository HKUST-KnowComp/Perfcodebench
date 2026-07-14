#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
inline uint64_t popcount64(uint64_t x) {
#if defined(__clang__) || defined(__GNUC__)
  return static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(x)));
#else
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
#endif
}
}  // namespace

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  const std::size_t n = words.size();
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    const uint64_t* data = words.data();
    for (std::size_t i = 0; i < n; ++i) {
      sum += popcount64(data[i]);
    }
    count = sum;
  }

  return count;
}
