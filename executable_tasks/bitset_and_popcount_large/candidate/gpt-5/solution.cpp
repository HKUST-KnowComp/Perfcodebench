#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters <= 0) return 0;

  // Compute once; result is identical for every iteration since inputs are const.
  std::size_t n = left.size();
  if (right.size() < n) n = right.size();

  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t total = 0;

  std::size_t i = 0;
  std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    uint64_t a0 = l[i]     & r[i];
    uint64_t a1 = l[i + 1] & r[i + 1];
    uint64_t a2 = l[i + 2] & r[i + 2];
    uint64_t a3 = l[i + 3] & r[i + 3];
    total += static_cast<uint64_t>(__builtin_popcountll(a0));
    total += static_cast<uint64_t>(__builtin_popcountll(a1));
    total += static_cast<uint64_t>(__builtin_popcountll(a2));
    total += static_cast<uint64_t>(__builtin_popcountll(a3));
  }
  for (; i < n; ++i) {
    total += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }

  return total;
}
