#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  // Match baseline behavior: if iters <= 0, return 0
  if (iters <= 0) {
    return 0;
  }

  const uint64_t* p = words.data();
  const std::size_t n = words.size();
  uint64_t cnt = 0;

  // Unroll by 8 for better ILP and reduced loop overhead
  std::size_t i = 0;
  const std::size_t limit = n & static_cast<std::size_t>(~(std::size_t(8) - 1));
  for (; i < limit; i += 8) {
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 0])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 1])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 2])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 3])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 4])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 5])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 6])));
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 7])));
  }
  for (; i < n; ++i) {
    cnt += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i])));
  }

  return cnt;
}
