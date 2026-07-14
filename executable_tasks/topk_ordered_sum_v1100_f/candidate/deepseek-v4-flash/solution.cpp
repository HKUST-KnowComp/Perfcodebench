#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  (void)iters; // unused
  if (k <= 0) return 0;
  std::vector<uint32_t> tmp = scores;
  // Use partial_sort to get top k in descending order. For tiny k, O(n log k).
  std::partial_sort(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
  uint64_t sum = 0;
  for (int i = 0; i < k; ++i) {
    sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
  }
  return sum;
}
