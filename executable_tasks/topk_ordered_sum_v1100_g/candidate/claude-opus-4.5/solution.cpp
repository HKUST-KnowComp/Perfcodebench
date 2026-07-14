#include "interface.h"

#include <algorithm>
#include <cstdint>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  if (k <= 0 || scores.empty()) {
    return 0;
  }
  
  const std::size_t n = scores.size();
  const std::size_t kk = static_cast<std::size_t>(k);
  const std::size_t actual_k = (kk < n) ? kk : n;
  
  std::vector<uint32_t> tmp;
  tmp.reserve(n);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp.assign(scores.begin(), scores.end());
    
    if (actual_k < n) {
      std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(actual_k), tmp.end(), std::greater<uint32_t>());
    }
    
    sum = 0;
    for (std::size_t i = 0; i < actual_k; ++i) {
      sum += static_cast<uint64_t>(tmp[i]);
    }
  }
  
  return sum;
}