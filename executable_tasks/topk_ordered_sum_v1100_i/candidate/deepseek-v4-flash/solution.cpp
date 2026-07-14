#include "interface.h"
#include <algorithm>
#include <cstdint>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::vector<uint32_t> tmp = scores;
    std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
    sum = 0;
    for (int i = 0; i < k; ++i) {
      sum += tmp[i];
    }
  }
  return sum;
}
