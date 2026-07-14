#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp;
  tmp.reserve(scores.size());
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    tmp = scores;
    if (k > 0 && k < static_cast<int>(tmp.size())) {
      std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), [](uint32_t a, uint32_t b) { return a > b; });
    }
    sum = 0;
    for (int i = 0; i < k; ++i) {
      sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
    }
  }
  return sum;
}