#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total = 0;
  const std::size_t n = left.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t cnt = 0;
    for (std::size_t i = 0; i < n; ++i) {
      cnt += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
    }
    total = cnt;
  }
  return total;
}