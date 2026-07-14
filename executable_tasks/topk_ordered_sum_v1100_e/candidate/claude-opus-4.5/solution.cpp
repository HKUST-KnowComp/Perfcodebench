#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <functional>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  const std::size_t kk = static_cast<std::size_t>(k);
  
  if (kk == 0 || n == 0) return 0;
  
  std::vector<uint32_t> tmp;
  tmp.reserve(n);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp.assign(scores.begin(), scores.end());
    
    if (kk >= n) {
      // Sum all elements
      sum = 0;
      const uint32_t* ptr = tmp.data();
      std::size_t i = 0;
      // Unrolled loop for better ILP
      for (; i + 4 <= n; i += 4) {
        sum += ptr[i] + ptr[i+1] + ptr[i+2] + ptr[i+3];
      }
      for (; i < n; ++i) {
        sum += ptr[i];
      }
    } else {
      // Use nth_element for O(n) partial sort
      std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(), std::greater<uint32_t>());
      
      // Sum the top k elements (now in tmp[0..k-1], unordered but all >= tmp[k])
      sum = 0;
      const uint32_t* ptr = tmp.data();
      std::size_t i = 0;
      // Prefetch and unroll for cache efficiency
      for (; i + 8 <= kk; i += 8) {
        if (i + 64 < kk) {
          __builtin_prefetch(ptr + i + 64, 0, 0);
        }
        sum += static_cast<uint64_t>(ptr[i]) + ptr[i+1] + ptr[i+2] + ptr[i+3]
             + ptr[i+4] + ptr[i+5] + ptr[i+6] + ptr[i+7];
      }
      for (; i < kk; ++i) {
        sum += ptr[i];
      }
    }
  }
  return sum;
}