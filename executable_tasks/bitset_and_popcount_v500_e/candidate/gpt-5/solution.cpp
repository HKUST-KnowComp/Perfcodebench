#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int /*iters*/) {
  // Compute AND-popcount once; correctness only depends on the final count.
  const std::size_t n = left.size() < right.size() ? left.size() : right.size();
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t total = 0;
  std::size_t i = 0;

  // Unroll by 8 for throughput.
  const std::size_t limit = n & ~static_cast<std::size_t>(7);
  for (; i < limit; i += 8) {
    uint64_t a0 = l[i + 0] & r[i + 0];
    uint64_t a1 = l[i + 1] & r[i + 1];
    uint64_t a2 = l[i + 2] & r[i + 2];
    uint64_t a3 = l[i + 3] & r[i + 3];
    uint64_t a4 = l[i + 4] & r[i + 4];
    uint64_t a5 = l[i + 5] & r[i + 5];
    uint64_t a6 = l[i + 6] & r[i + 6];
    uint64_t a7 = l[i + 7] & r[i + 7];

    total += static_cast<uint64_t>(__builtin_popcountll(a0));
    total += static_cast<uint64_t>(__builtin_popcountll(a1));
    total += static_cast<uint64_t>(__builtin_popcountll(a2));
    total += static_cast<uint64_t>(__builtin_popcountll(a3));
    total += static_cast<uint64_t>(__builtin_popcountll(a4));
    total += static_cast<uint64_t>(__builtin_popcountll(a5));
    total += static_cast<uint64_t>(__builtin_popcountll(a6));
    total += static_cast<uint64_t>(__builtin_popcountll(a7));
  }

  for (; i < n; ++i) {
    total += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }

  return total;
}
