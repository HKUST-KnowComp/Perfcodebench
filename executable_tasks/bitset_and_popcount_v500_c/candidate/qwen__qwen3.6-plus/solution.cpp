#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  (void)iters;
  uint64_t count = 0;
  std::size_t n = left.size();
  for (std::size_t i = 0; i < n; ++i) {
    count += __builtin_popcountll(left[i] & right[i]);
  }
  return count;
}