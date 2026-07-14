#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t n = left.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    std::size_t i = 0;

    // 4× unrolled loop
    for (; i + 3 < n; i += 4) {
      local += __builtin_popcountll(left[i]   & right[i]);
      local += __builtin_popcountll(left[i+1] & right[i+1]);
      local += __builtin_popcountll(left[i+2] & right[i+2]);
      local += __builtin_popcountll(left[i+3] & right[i+3]);
    }

    // tail
    for (; i < n; ++i) {
      local += __builtin_popcountll(left[i] & right[i]);
    }

    count = local;
  }
  return count;
}