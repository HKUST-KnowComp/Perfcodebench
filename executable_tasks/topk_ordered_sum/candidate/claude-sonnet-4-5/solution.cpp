#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp = scores;
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    if (iter > 0) {
      tmp = scores;
    }
    
    // Use nth_element to partition: elements >= kth largest will be in [0, k)
    // This is O(n) average case vs O(n log n) for full sort
    std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
    
    sum = 0;
    for (int i = 0; i < k; ++i) {
      sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
    }
  }
  
  return sum;
}