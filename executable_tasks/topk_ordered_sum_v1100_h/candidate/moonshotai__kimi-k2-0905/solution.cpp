#include "interface.h"

#include <algorithm>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  if (k <= 0) return 0;
  const std::size_t n = scores.size();
  if (static_cast<std::size_t>(k) > n) k = static_cast<int>(n);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // pick the k-th largest element as pivot (0-based rank = k-1)
    std::vector<uint32_t> tmp = scores;
    auto first = tmp.begin();
    auto nth   = tmp.begin() + (k - 1);
    auto last  = tmp.end();
    std::nth_element(first, nth, last, std::greater<uint32_t>());
    const uint32_t pivot = *nth;

    // sum all elements >= pivot (exactly k of them)
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t v = tmp[i];
      if (v >= pivot) {          // handles duplicates correctly
        sum += static_cast<uint64_t>(v);
      }
    }
  }
  return sum;
}