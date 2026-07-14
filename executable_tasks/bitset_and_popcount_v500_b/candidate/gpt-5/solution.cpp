#include "interface.h"

#include <vector>
#include <cstdint>

static inline uint64_t and_popcount_sum(const uint64_t* l, const uint64_t* r, std::size_t n) {
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  std::size_t limit = n & static_cast<std::size_t>(~3ULL);
  for (; i < limit; i += 4) {
    uint64_t v0 = l[i]     & r[i];
    uint64_t v1 = l[i + 1] & r[i + 1];
    uint64_t v2 = l[i + 2] & r[i + 2];
    uint64_t v3 = l[i + 3] & r[i + 3];
    s0 += static_cast<uint64_t>(__builtin_popcountll(v0));
    s1 += static_cast<uint64_t>(__builtin_popcountll(v1));
    s2 += static_cast<uint64_t>(__builtin_popcountll(v2));
    s3 += static_cast<uint64_t>(__builtin_popcountll(v3));
  }
  uint64_t sum = (s0 + s1) + (s2 + s3);
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }
  return sum;
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters <= 0) return 0;
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();
  std::size_t n = left.size();

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = and_popcount_sum(l, r, n);
  }
  return count;
}
