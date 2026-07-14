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

  uint64_t sum = 0;

  // Fast path: summing all elements avoids any copying/selection work.
  if (kk == n) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      for (std::size_t i = 0; i < n; ++i) {
        s += static_cast<uint64_t>(scores[i]);
      }
      sum = s;
    }
    return sum;
  }

  std::vector<uint32_t> tmp(scores.begin(), scores.end());

  for (int iter = 0; iter < iters; ++iter) {
    if (iter != 0) {
      tmp.assign(scores.begin(), scores.end());
    }

    auto mid = tmp.begin() + static_cast<std::ptrdiff_t>(kk);
    std::nth_element(tmp.begin(), mid, tmp.end(), std::greater<uint32_t>());

    uint64_t s = 0;
    for (std::size_t i = 0; i < kk; ++i) {
      s += static_cast<uint64_t>(tmp[i]);
    }
    sum = s;
  }

  return sum;
}
