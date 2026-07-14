#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp;
  uint64_t sum = 0;
  
  // Fast path for k=1
  if (k == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      sum = *std::max_element(scores.begin(), scores.end());
    }
    return sum;
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp = scores;
    
    // Use nth_element to partition: O(n) instead of O(n log n)
    // After this, tmp[k-1] is the k-th largest, and all elements
    // in [0, k) are >= all elements in [k, end)
    std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
    
    // Sort only the top k elements
    std::sort(tmp.begin(), tmp.begin() + k, std::greater<uint32_t>());
    
    sum = 0;
    for (int i = 0; i < k; ++i) {
      sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
    }
  }
  return sum;
}