#include "interface.h"

#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  (void)iters; // Result is independent of iters per contract and baseline behavior

  const uint64_t* p = words.data();
  std::size_t n = words.size();

  // Use multiple accumulators to reduce dependency chains
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    s0 += static_cast<uint64_t>(__builtin_popcountll(p[i + 0]));
    s1 += static_cast<uint64_t>(__builtin_popcountll(p[i + 1]));
    s2 += static_cast<uint64_t>(__builtin_popcountll(p[i + 2]));
    s3 += static_cast<uint64_t>(__builtin_popcountll(p[i + 3]));
  }

  uint64_t total = s0 + s1 + s2 + s3;
  for (; i < n; ++i) {
    total += static_cast<uint64_t>(__builtin_popcountll(p[i]));
  }

  return total;
}
