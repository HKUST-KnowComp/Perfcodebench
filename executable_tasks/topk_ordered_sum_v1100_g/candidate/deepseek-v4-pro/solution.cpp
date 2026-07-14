#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  (void)iters; // unused
  const std::size_t n = scores.size();
  if (n == 0 || k <= 0) {
    return 0;
  }
  if (static_cast<std::size_t>(k) >= n) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(scores[i]);
    }
    return sum;
  }
  std::vector<uint32_t> tmp(scores);
  std::nth_element(tmp.begin(), tmp.begin() + (k - 1), tmp.end(), std::greater<uint32_t>());
  uint64_t sum = 0;
  for (int i = 0; i < k; ++i) {
    sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
  }
  return sum;
}
