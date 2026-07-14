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

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  const std::size_t n = words.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    std::size_t i = 0;

    for (; i + 4 <= n; i += 4) {
      sum += popcount64(words[i]);
      sum += popcount64(words[i + 1]);
      sum += popcount64(words[i + 2]);
      sum += popcount64(words[i + 3]);
    }
    for (; i < n; ++i) {
      sum += popcount64(words[i]);
    }

    count = sum;
  }

  return count;
}
