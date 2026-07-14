#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t n = left.size();
  const uint64_t* __restrict lp = left.data();
  const uint64_t* __restrict rp = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for better ILP
    for (; i + 4 <= n; i += 4) {
      count += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
      count += static_cast<uint64_t>(__builtin_popcountll(lp[i+1] & rp[i+1]));
      count += static_cast<uint64_t>(__builtin_popcountll(lp[i+2] & rp[i+2]));
      count += static_cast<uint64_t>(__builtin_popcountll(lp[i+3] & rp[i+3]));
    }
    
    // Handle remaining words
    for (; i < n; ++i) {
      count += static_cast<uint64_t>(__builtin_popcountll(lp[i] & rp[i]));
    }
  }
  return count;
}