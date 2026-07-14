#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t size = left.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    std::size_t i = 0;
    
    // Process 4 words at a time for better ILP
    const std::size_t unroll_limit = size & ~3ULL;
    for (; i < unroll_limit; i += 4) {
      acc0 += __builtin_popcountll(left[i] & right[i]);
      acc1 += __builtin_popcountll(left[i + 1] & right[i + 1]);
      acc2 += __builtin_popcountll(left[i + 2] & right[i + 2]);
      acc3 += __builtin_popcountll(left[i + 3] & right[i + 3]);
    }
    
    // Handle remaining words
    for (; i < size; ++i) {
      acc0 += __builtin_popcountll(left[i] & right[i]);
    }
    
    count = acc0 + acc1 + acc2 + acc3;
  }
  
  return count;
}