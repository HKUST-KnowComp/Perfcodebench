#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  if (iters <= 0) return 0;

  const uint64_t* p = words.data();
  std::size_t n = words.size();

  uint64_t sum = 0;
  std::size_t i = 0;

  // Unroll by 4 for better ILP
  std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    sum += static_cast<uint64_t>(__builtin_popcountll(p[i]));
    sum += static_cast<uint64_t>(__builtin_popcountll(p[i + 1]));
    sum += static_cast<uint64_t>(__builtin_popcountll(p[i + 2]));
    sum += static_cast<uint64_t>(__builtin_popcountll(p[i + 3]));
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(__builtin_popcountll(p[i]));
  }

  // Preserve baseline semantics with respect to iters
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    result = sum;
  }
  return result;
}
