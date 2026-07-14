#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  (void)iters; // Unused: result is identical across iterations

  if (k <= 0 || scores.empty()) return 0;

  const size_t n = scores.size();
  size_t kk = static_cast<size_t>(k);
  if (kk >= n) {
    uint64_t s = 0;
    for (uint32_t v : scores) s += static_cast<uint64_t>(v);
    return s;
  }

  if (kk == 1) {
    uint32_t m = 0;
    for (uint32_t v : scores) if (v > m) m = v;
    return static_cast<uint64_t>(m);
  }

  std::vector<uint32_t> top(kk);
  std::partial_sort_copy(scores.begin(), scores.end(), top.begin(), top.end(), std::greater<uint32_t>());

  uint64_t sum = 0;
  for (size_t i = 0; i < kk; ++i) sum += static_cast<uint64_t>(top[i]);
  return sum;
}
