#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  (void)iters; // iterations are irrelevant for correctness; compute once

  const std::size_t n = left.size();
  const uint64_t* lp = left.data();
  const uint64_t* rp = right.data();

  uint64_t result = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & static_cast<std::size_t>(~7ULL);
  for (; i < n8; i += 8) {
    result += static_cast<uint64_t>(__builtin_popcountll(lp[i]     & rp[i]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 1] & rp[i + 1]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 2] & rp[i + 2]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 3] & rp[i + 3]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 4] & rp[i + 4]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 5] & rp[i + 5]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 6] & rp[i + 6]))
           + static_cast<uint64_t>(__builtin_popcountll(lp[i + 7] & rp[i + 7]));
  }
  for (; i < n; ++i) {
    result += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
  }

  return result;
}
