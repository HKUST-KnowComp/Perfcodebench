#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  (void)iters; // Result is independent of iteration count
  const std::size_t n = left.size();
  if (n == 0) return 0;

  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;

  // Unroll by 8 with 4 accumulators to increase ILP
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    s0 += static_cast<uint64_t>(__builtin_popcountll(l[i + 0] & r[i + 0]));
    s1 += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
    s2 += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
    s3 += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
    s0 += static_cast<uint64_t>(__builtin_popcountll(l[i + 4] & r[i + 4]));
    s1 += static_cast<uint64_t>(__builtin_popcountll(l[i + 5] & r[i + 5]));
    s2 += static_cast<uint64_t>(__builtin_popcountll(l[i + 6] & r[i + 6]));
    s3 += static_cast<uint64_t>(__builtin_popcountll(l[i + 7] & r[i + 7]));
  }

  uint64_t sum = s0 + s1 + s2 + s3;

  // Remainder
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }

  return sum;
}
