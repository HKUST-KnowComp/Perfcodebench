#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  const std::size_t kk = static_cast<std::size_t>(k);
  
  if (kk == 0 || n == 0) {
    return 0;
  }
  
  std::vector<uint32_t> tmp;
  tmp.reserve(n);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp.assign(scores.begin(), scores.end());
    
    if (kk >= n) {
      // Sum all elements
      sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        sum += tmp[i];
      }
    } else {
      // Partial sort: partition so that the k largest are in [0, k)
      std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(), std::greater<uint32_t>());
      
      // Sum the top k elements
      sum = 0;
      for (std::size_t i = 0; i < kk; ++i) {
        sum += tmp[i];
      }
    }
  }
  
  return sum;
}