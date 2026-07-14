#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  // Preserve baseline semantics: if iters == 0, return 0
  if (iters <= 0) return 0;

  const std::size_t n = xs.size();
  if (n <= 1) return 0;

  // Sort copies of coordinates
  std::vector<int32_t> sx(xs.begin(), xs.end());
  std::vector<int32_t> sy(ys.begin(), ys.end());
  std::sort(sx.begin(), sx.end());
  std::sort(sy.begin(), sy.end());

  // Compute sum of pairwise absolute differences for each axis
  int64_t total_x = 0;
  int64_t total_y = 0;
  int64_t prefix_x = 0;
  int64_t prefix_y = 0;

  for (std::size_t k = 0; k < n; ++k) {
    int64_t vx = static_cast<int64_t>(sx[k]);
    int64_t vy = static_cast<int64_t>(sy[k]);
    total_x += static_cast<int64_t>(k) * vx - prefix_x;
    total_y += static_cast<int64_t>(k) * vy - prefix_y;
    prefix_x += vx;
    prefix_y += vy;
  }

  int64_t total = total_x + total_y;
  return total;
}
