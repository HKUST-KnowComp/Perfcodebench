#include "interface.h"

#include <algorithm>
#include <cstddef>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  // Clamp k to valid range [0, n]
  int kk = k;
  if (kk < 0) kk = 0;
  if (static_cast<std::size_t>(kk) > n) kk = static_cast<int>(n);

  uint64_t sum = 0;
  if (iters <= 0) return 0;

  // Reusable buffer for in-place selection when needed
  std::vector<uint32_t> tmp;
  if (kk > 0 && static_cast<std::size_t>(kk) < n) {
    tmp.resize(n);
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (kk == 0) {
      sum = 0;
      continue;
    }
    if (static_cast<std::size_t>(kk) == n) {
      // Sum all elements directly from scores without copying
      uint64_t s = 0;
      for (std::size_t i = 0; i < n; ++i) {
        s += static_cast<uint64_t>(scores[i]);
      }
      sum = s;
      continue;
    }

    // Copy input to temporary buffer for in-place selection
    std::copy(scores.begin(), scores.end(), tmp.begin());

    uint64_t s = 0;
    const std::size_t ksz = static_cast<std::size_t>(kk);
    if (ksz <= n - ksz) {
      // Place the k largest elements in the first k positions (unordered)
      std::nth_element(
          tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(ksz), tmp.end(),
          [](uint32_t a, uint32_t b) { return a > b; });
      for (std::size_t i = 0; i < ksz; ++i) {
        s += static_cast<uint64_t>(tmp[i]);
      }
    } else {
      // Place the (n-k) smallest elements in the first (n-k) positions
      const std::size_t pos = n - ksz;
      std::nth_element(
          tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(pos), tmp.end());
      for (std::size_t i = pos; i < n; ++i) {
        s += static_cast<uint64_t>(tmp[i]);
      }
    }
    sum = s;
  }

  return sum;
}
