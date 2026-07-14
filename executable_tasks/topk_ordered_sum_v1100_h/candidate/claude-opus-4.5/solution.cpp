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

    if (kk >= n) {
      // sum all
      sum = 0;
      const uint32_t* __restrict__ p = tmp.data();
      const std::size_t nn = n;
      #pragma omp simd reduction(+:sum)
      for (std::size_t i = 0; i < nn; ++i) {
        sum += static_cast<uint64_t>(p[i]);
      }
    } else {
      // partial selection: put largest kk elements in [0, kk)
      std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(), std::greater<uint32_t>());

      sum = 0;
      const uint32_t* __restrict__ p = tmp.data();
      #pragma omp simd reduction(+:sum)
      for (std::size_t i = 0; i < kk; ++i) {
        sum += static_cast<uint64_t>(p[i]);
      }
    }
  }
  return sum;
}
