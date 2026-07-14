#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  if (n == 0 || k <= 0) return 0ULL;
  if (k > static_cast<int>(n)) k = static_cast<int>(n);

  uint64_t sum = 0;

  // Preallocate buffers to avoid per-iteration allocations
  std::vector<uint32_t> tmp; tmp.reserve(n);
  std::vector<uint32_t> topk; topk.reserve(static_cast<std::size_t>(k));

  // Heuristic: use partial_sort_copy when k is small relative to n, else nth_element
  const bool use_partial = (static_cast<std::size_t>(k) * 64 <= n);

  for (int iter = 0; iter < iters; ++iter) {
    if (use_partial) {
      topk.resize(static_cast<std::size_t>(k));
      std::partial_sort_copy(scores.begin(), scores.end(), topk.begin(), topk.end(), std::greater<uint32_t>());
      uint64_t s = 0;
      const uint32_t* p = topk.data();
      for (int i = 0; i < k; ++i) s += static_cast<uint64_t>(p[static_cast<std::size_t>(i)]);
      sum = s;
    } else {
      if (static_cast<std::size_t>(k) == n) {
        uint64_t s = 0;
        const uint32_t* p = scores.data();
        for (std::size_t i = 0; i < n; ++i) s += static_cast<uint64_t>(p[i]);
        sum = s;
      } else {
        tmp.resize(n);
        std::copy(scores.begin(), scores.end(), tmp.begin());
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        uint64_t s = 0;
        const uint32_t* p = tmp.data();
        for (int i = 0; i < k; ++i) s += static_cast<uint64_t>(p[static_cast<std::size_t>(i)]);
        sum = s;
      }
    }
  }
  return sum;
}
