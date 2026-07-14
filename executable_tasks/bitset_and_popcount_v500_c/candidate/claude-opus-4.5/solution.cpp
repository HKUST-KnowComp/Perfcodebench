#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t n = left.size();
  const uint64_t* __restrict lp = left.data();
  const uint64_t* __restrict rp = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t c = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      c += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
      c += static_cast<uint64_t>(__builtin_popcountll(lp[i+1] & rp[i+1]));
      c += static_cast<uint64_t>(__builtin_popcountll(lp[i+2] & rp[i+2]));
      c += static_cast<uint64_t>(__builtin_popcountll(lp[i+3] & rp[i+3]));
    }
    
    // Handle remaining words
    for (; i < n; ++i) {
      c += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
    }
    
    count = c;
  }
  return count;
}