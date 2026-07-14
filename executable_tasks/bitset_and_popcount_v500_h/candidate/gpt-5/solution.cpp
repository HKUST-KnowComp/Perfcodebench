#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int /*iters*/) {
  // Compute the popcount of (left & right) across overlapping words only.
  // Ignore iters: correctness depends only on the final AND-popcount.
  const std::size_t n = left.size() < right.size() ? left.size() : right.size();
  if (n == 0) return 0;

  const uint64_t* L = left.data();
  const uint64_t* R = right.data();

  uint64_t count = 0;
  std::size_t i = 0;

  // Unroll by 8 for throughput
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 0] & R[i + 0]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 1] & R[i + 1]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 2] & R[i + 2]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 3] & R[i + 3]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 4] & R[i + 4]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 5] & R[i + 5]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 6] & R[i + 6]));
    count += static_cast<uint64_t>(__builtin_popcountll(L[i + 7] & R[i + 7]));
  }

  for (; i < n; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(L[i] & R[i]));
  }

  return count;
}
