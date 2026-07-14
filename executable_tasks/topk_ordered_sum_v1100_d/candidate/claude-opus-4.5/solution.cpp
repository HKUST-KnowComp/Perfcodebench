#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  const std::size_t kk = static_cast<std::size_t>(k);
  if (kk == 0 || n == 0) return 0;

  std::vector<uint32_t> tmp;
  tmp.reserve(n);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    tmp.assign(scores.begin(), scores.end());

    if (kk < n) {
      std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(), std::greater<uint32_t>());
    }

    const uint32_t* __restrict ptr = tmp.data();
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t limit = kk & ~std::size_t(3);
    for (; i < limit; i += 4) {
      s0 += ptr[i];
      s1 += ptr[i + 1];
      s2 += ptr[i + 2];
      s3 += ptr[i + 3];
    }
    sum = s0 + s1 + s2 + s3;
    for (; i < kk; ++i) {
      sum += ptr[i];
    }
  }
  return sum;
}
