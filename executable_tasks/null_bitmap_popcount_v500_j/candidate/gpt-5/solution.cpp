#include "interface.h"

#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  // Match baseline behavior: if no iterations requested, return 0.
  if (iters <= 0) {
    return 0;
  }

  const uint64_t* p = words.data();
  std::size_t n = words.size();
  uint64_t sum = 0;

  // Unroll by 8 for better ILP and reduced loop overhead.
  std::size_t i = 0;
  std::size_t limit = n & static_cast<std::size_t>(~7ULL);
  for (; i < limit; i += 8) {
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 0])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 1])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 2])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 3])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 4])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 5])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 6])));
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 7])));
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i])));
  }

  return sum;
}
