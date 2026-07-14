#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

static inline int64_t sum_abs_1d(const std::vector<int32_t>& arr) {
  const std::size_t n = arr.size();
  if (n <= 1) return 0;

  std::vector<int32_t> v(arr.begin(), arr.end());
  std::sort(v.begin(), v.end());

  __int128 acc = 0;
  const long long nn = static_cast<long long>(n);
  for (std::size_t i = 0; i < n; ++i) {
    long long coef = static_cast<long long>(2 * static_cast<long long>(i) - nn + 1);
    acc += static_cast<__int128>(v[i]) * static_cast<__int128>(coef);
  }
  return static_cast<int64_t>(acc);
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  (void)iters; // Intentionally ignored to preserve baseline behavior
  int64_t sumx = sum_abs_1d(xs);
  int64_t sumy = sum_abs_1d(ys);
  __int128 total = static_cast<__int128>(sumx) + static_cast<__int128>(sumy);
  return static_cast<int64_t>(total);
}
