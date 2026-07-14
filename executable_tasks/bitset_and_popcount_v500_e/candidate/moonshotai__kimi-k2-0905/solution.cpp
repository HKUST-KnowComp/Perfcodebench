#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    for (std::size_t i = 0; i < n; ++i) {
      local += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
    }
    count = local;   // keep final iteration’s result
  }
  return count;
}