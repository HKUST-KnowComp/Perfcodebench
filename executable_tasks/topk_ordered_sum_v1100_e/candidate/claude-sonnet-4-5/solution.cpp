#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp;
  tmp.reserve(scores.size());
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp = scores;
    
    // For small k relative to n, partial_sort can be faster
    // For larger k, nth_element + sum is better
    if (k < 100 || k * 10 < static_cast<int>(tmp.size())) {
      std::partial_sort(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
    } else {
      std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
    }
    
    sum = 0;
    for (int i = 0; i < k; ++i) {
      sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
    }
  }
  return sum;
}