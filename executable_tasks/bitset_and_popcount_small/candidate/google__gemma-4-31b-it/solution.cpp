#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const size_t size = left.size();
  
  // The original code resets 'count' inside the loop and returns the result of the last iteration.
  // However, the logic suggests it wants the popcount of the intersection.
  // Based on the baseline: count = 0 is inside the iters loop, so it returns the result of the final iteration.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    for (size_t i = 0; i < size; ++i) {
      current_iter_count += __builtin_popcountll(left[i] & right[i]);
    }
    total_count = current_iter_count;
  }
  
  return total_count;
}