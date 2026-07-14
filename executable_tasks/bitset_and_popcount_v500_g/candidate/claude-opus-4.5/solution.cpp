#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  const uint64_t* __restrict lp = left.data();
  const uint64_t* __restrict rp = right.data();
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      c0 += static_cast<uint64_t>(__builtin_popcountll(lp[i]     & rp[i]));
      c1 += static_cast<uint64_t>(__builtin_popcountll(lp[i + 1] & rp[i + 1]));
      c2 += static_cast<uint64_t>(__builtin_popcountll(lp[i + 2] & rp[i + 2]));
      c3 += static_cast<uint64_t>(__builtin_popcountll(lp[i + 3] & rp[i + 3]));
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      c0 += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
    }
    
    count = c0 + c1 + c2 + c3;
  }
  
  return count;
}