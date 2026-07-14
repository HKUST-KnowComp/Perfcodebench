#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Optimized implementation using hardware popcount instructions.
 * The original implementation had O(N*64) complexity and excessive memory allocation.
 * This version performs O(N) operations per iteration using __builtin_popcountll.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const size_t n = left.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    for (size_t i = 0; i < n; ++i) {
      // Perform bitwise AND and count set bits using hardware instruction
      current_iter_count += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
    }
    total_count = current_iter_count;
  }
  
  return total_count;
}