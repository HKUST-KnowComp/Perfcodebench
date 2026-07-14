#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  if (iters <= 0) return 0;

  const uint64_t* p = words.data();
  std::size_t n = words.size();
  uint64_t count = 0;

  std::size_t i = 0;
  std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 0])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 1])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 2])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 3])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 4])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 5])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 6])));
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i + 7])));
  }
  for (; i < n; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(static_cast<unsigned long long>(p[i])));
  }

  return count;
}
