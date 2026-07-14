#include "interface.h"

#include <algorithm>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  std::vector<uint32_t> tmp;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    tmp = scores;
    std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
    sum = std::accumulate(tmp.begin(), tmp.begin() + k, uint64_t{0});
  }
  return sum;
}