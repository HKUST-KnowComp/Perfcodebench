#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  const std::size_t n = xs.size();
  if (n < 2) return 0;

  // Copy and sort coordinates
  std::vector<int32_t> sx(xs.begin(), xs.end());
  std::vector<int32_t> sy(ys.begin(), ys.end());
  std::sort(sx.begin(), sx.end());
  std::sort(sy.begin(), sy.end());

  const int64_t N = static_cast<int64_t>(n);
  int64_t sumx = 0;
  int64_t sumy = 0;

  // Sum_{k} v[k] * (2k - n + 1) for both dimensions
  for (std::size_t i = 0; i < n; ++i) {
    int64_t coef = (static_cast<int64_t>(i) << 1) - N + 1;
    sumx += static_cast<int64_t>(sx[i]) * coef;
    sumy += static_cast<int64_t>(sy[i]) * coef;
  }

  return sumx + sumy;
}
