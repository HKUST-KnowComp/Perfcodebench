#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t n = left.size();
  const uint64_t* __restrict lptr = left.data();
  const uint64_t* __restrict rptr = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      sum += static_cast<uint64_t>(__builtin_popcountll(lptr[i] & rptr[i]));
      sum += static_cast<uint64_t>(__builtin_popcountll(lptr[i+1] & rptr[i+1]));
      sum += static_cast<uint64_t>(__builtin_popcountll(lptr[i+2] & rptr[i+2]));
      sum += static_cast<uint64_t>(__builtin_popcountll(lptr[i+3] & rptr[i+3]));
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(__builtin_popcountll(lptr[i] & rptr[i]));
    }
    
    count = sum;
  }
  return count;
}