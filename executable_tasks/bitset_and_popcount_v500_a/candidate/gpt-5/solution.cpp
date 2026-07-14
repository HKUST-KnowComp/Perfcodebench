#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = left.size() < right.size() ? left.size() : right.size();
  if (n == 0) {
    return 0;
  }

  const uint64_t* lp = left.data();
  const uint64_t* rp = right.data();

  uint64_t count = 0;
  std::size_t i = 0;
  const std::size_t limit = n & ~static_cast<std::size_t>(7);

  for (; i < limit; i += 8) {
    uint64_t a0 = lp[i] & rp[i];
    uint64_t a1 = lp[i + 1] & rp[i + 1];
    uint64_t a2 = lp[i + 2] & rp[i + 2];
    uint64_t a3 = lp[i + 3] & rp[i + 3];
    uint64_t a4 = lp[i + 4] & rp[i + 4];
    uint64_t a5 = lp[i + 5] & rp[i + 5];
    uint64_t a6 = lp[i + 6] & rp[i + 6];
    uint64_t a7 = lp[i + 7] & rp[i + 7];

    count += static_cast<uint64_t>(__builtin_popcountll(a0));
    count += static_cast<uint64_t>(__builtin_popcountll(a1));
    count += static_cast<uint64_t>(__builtin_popcountll(a2));
    count += static_cast<uint64_t>(__builtin_popcountll(a3));
    count += static_cast<uint64_t>(__builtin_popcountll(a4));
    count += static_cast<uint64_t>(__builtin_popcountll(a5));
    count += static_cast<uint64_t>(__builtin_popcountll(a6));
    count += static_cast<uint64_t>(__builtin_popcountll(a7));
  }

  for (; i < n; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
  }

  return count;
}
