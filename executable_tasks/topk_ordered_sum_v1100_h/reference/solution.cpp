#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    tmp = scores;
    std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
    sum = 0;
    for (int i = 0; i < k; ++i) {
      sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
    }
  }
  return sum;
}
