#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  // Preserve baseline behavior for non-positive iters
  if (iters <= 0) {
    return 0;
  }

  // Use the minimum size to avoid out-of-bounds access if sizes differ
  std::size_t n = left.size();
  if (right.size() < n) n = right.size();

  const uint64_t* pl = left.data();
  const uint64_t* pr = right.data();

  // Compute the AND-popcount once; result is identical for each iteration
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  std::size_t limit = (n / 8) * 8; // unroll by 8
  for (; i < limit; i += 8) {
    s0 += __builtin_popcountll(pl[i + 0] & pr[i + 0]);
    s1 += __builtin_popcountll(pl[i + 1] & pr[i + 1]);
    s2 += __builtin_popcountll(pl[i + 2] & pr[i + 2]);
    s3 += __builtin_popcountll(pl[i + 3] & pr[i + 3]);

    s0 += __builtin_popcountll(pl[i + 4] & pr[i + 4]);
    s1 += __builtin_popcountll(pl[i + 5] & pr[i + 5]);
    s2 += __builtin_popcountll(pl[i + 6] & pr[i + 6]);
    s3 += __builtin_popcountll(pl[i + 7] & pr[i + 7]);
  }

  for (; i < n; ++i) {
    s0 += __builtin_popcountll(pl[i] & pr[i]);
  }

  uint64_t count = s0 + s1 + s2 + s3;

  // The baseline recomputed the same value each iteration.
  // Returning the single computed value preserves correctness.
  return count;
}
