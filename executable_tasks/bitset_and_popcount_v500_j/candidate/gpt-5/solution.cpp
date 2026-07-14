#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t popcount_and_sum(const uint64_t* __restrict l, const uint64_t* __restrict r, std::size_t n) {
  uint64_t c = 0;
  std::size_t i = 0;
  std::size_t limit = n & ~static_cast<std::size_t>(3);
  for (; i < limit; i += 4) {
    c += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    c += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
    c += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
    c += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
  }
  for (; i < n; ++i) {
    c += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }
  return c;
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  const uint64_t* __restrict l = left.data();
  const uint64_t* __restrict r = right.data();

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = popcount_and_sum(l, r, n);
  }
  return count;
}
