#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  if (iters <= 0) return 0;
  if (k <= 0 || n == 0) return 0;

  std::size_t kk = static_cast<std::size_t>(k);
  if (kk > n) kk = n;

  std::vector<uint32_t> tmp(scores.begin(), scores.end());
  uint64_t sum = 0;

  if (kk == n) {
    uint64_t total = 0;
    for (std::size_t i = 0; i < n; ++i) total += static_cast<uint64_t>(scores[i]);
    for (int iter = 0; iter < iters; ++iter) sum = total;
    return sum;
  }

  for (int iter = 0; iter < iters; ++iter) {
    std::copy(scores.begin(), scores.end(), tmp.begin());
    std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(),
                     std::greater<uint32_t>());

    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < kk; ++i) {
      local_sum += static_cast<uint64_t>(tmp[i]);
    }
    sum = local_sum;
  }

  return sum;
}
