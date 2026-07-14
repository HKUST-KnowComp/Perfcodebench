#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const uint64_t* __restrict__ l = left.data();
  const uint64_t* __restrict__ r = right.data();
  const std::size_t n = left.size();
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      c0 += static_cast<uint64_t>(__builtin_popcountll(l[i]     & r[i]));
      c1 += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
      c2 += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
      c3 += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
    }
    
    // Handle remaining words
    for (; i < n; ++i) {
      c0 += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    }
    
    count = c0 + c1 + c2 + c3;
  }
  
  return count;
}