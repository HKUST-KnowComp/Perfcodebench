#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  if (n == 0) return 0;
  uint64_t count = 0;
  const uint64_t* const left_data = left.data();
  const uint64_t* const right_data = right.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_count = 0;
    for (std::size_t i = 0; i < n; ++i) {
      local_count += __builtin_popcountll(left_data[i] & right_data[i]);
    }
    count = local_count;
  }
  return count;
}