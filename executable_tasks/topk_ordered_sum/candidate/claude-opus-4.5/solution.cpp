#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  if (k <= 0 || scores.empty()) {
    return 0;
  }
  
  const std::size_t n = scores.size();
  const std::size_t kk = static_cast<std::size_t>(k) > n ? n : static_cast<std::size_t>(k);
  
  std::vector<uint32_t> tmp;
  tmp.reserve(n);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    tmp.assign(scores.begin(), scores.end());
    
    // Partial sort: partition so that the largest kk elements are in [0, kk)
    std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(), std::greater<uint32_t>());
    
    // Sum the top-k elements with unrolled accumulation
    sum = 0;
    const uint32_t* ptr = tmp.data();
    std::size_t i = 0;
    
    // Unroll by 4 for better ILP
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    const std::size_t limit = kk & ~static_cast<std::size_t>(3);
    for (; i < limit; i += 4) {
      s0 += ptr[i];
      s1 += ptr[i + 1];
      s2 += ptr[i + 2];
      s3 += ptr[i + 3];
    }
    sum = s0 + s1 + s2 + s3;
    
    // Handle remainder
    for (; i < kk; ++i) {
      sum += ptr[i];
    }
  }
  
  return sum;
}
