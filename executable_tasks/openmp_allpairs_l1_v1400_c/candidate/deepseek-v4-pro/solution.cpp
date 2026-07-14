#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  if (iters <= 0) return 0;
  const std::size_t n = xs.size();
  if (n <= 1) return 0;

  // Copy and sort x coordinates
  std::vector<int32_t> x_sorted = xs;
  std::sort(x_sorted.begin(), x_sorted.end());
  int64_t sum_x = 0;
  for (std::size_t i = 0; i < n; ++i) {
    sum_x += static_cast<int64_t>(x_sorted[i]) * (2LL * i - n + 1);
  }

  // Copy and sort y coordinates
  std::vector<int32_t> y_sorted = ys;
  std::sort(y_sorted.begin(), y_sorted.end());
  int64_t sum_y = 0;
  for (std::size_t i = 0; i < n; ++i) {
    sum_y += static_cast<int64_t>(y_sorted[i]) * (2LL * i - n + 1);
  }

  return sum_x + sum_y;
}