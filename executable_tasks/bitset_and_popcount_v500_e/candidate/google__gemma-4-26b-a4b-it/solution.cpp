#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const std::size_t n = left.size();
  
  // Pre-fetch pointers to avoid vector overhead in the hot loop
  const uint64_t* l_ptr = left.data();
  const uint64_t* r_ptr = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    for (std::size_t i = 0; i < n; ++i) {
      // Perform bitwise AND and use hardware-accelerated popcount
      current_iter_count += static_cast<uint64_t>(__builtin_popcountll(l_ptr[i] & r_ptr[i]));
    }
    // The task implies we return the count from the LAST iteration based on the baseline logic
    total_count = current_iter_count;
  }
  
  return total_count;
}