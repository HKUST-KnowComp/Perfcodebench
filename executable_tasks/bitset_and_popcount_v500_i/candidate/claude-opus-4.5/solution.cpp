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
    count = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      count += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
      count += static_cast<uint64_t>(__builtin_popcountll(left[i + 1] & right[i + 1]));
      count += static_cast<uint64_t>(__builtin_popcountll(left[i + 2] & right[i + 2]));
      count += static_cast<uint64_t>(__builtin_popcountll(left[i + 3] & right[i + 3]));
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      count += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
    }
  }
  
  return count;
}