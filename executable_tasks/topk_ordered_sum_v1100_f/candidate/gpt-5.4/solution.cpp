#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  (void)iters;

  const std::size_t n = scores.size();
  if (k <= 0 || n == 0) return 0;

  std::size_t kk = static_cast<std::size_t>(k);
  if (kk >= n) {
    uint64_t sum = 0;
    for (uint32_t v : scores) sum += static_cast<uint64_t>(v);
    return sum;
  }

  std::vector<uint32_t> tmp(scores.begin(), scores.end());
  auto mid = tmp.begin() + static_cast<std::ptrdiff_t>(kk);
  std::nth_element(tmp.begin(), mid, tmp.end(), std::greater<uint32_t>());

  uint64_t sum = 0;
  for (std::size_t i = 0; i < kk; ++i) {
    sum += static_cast<uint64_t>(tmp[i]);
  }
  return sum;
}
