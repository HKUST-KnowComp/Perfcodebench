#include "interface.h"

#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  const std::size_t n = xs.size();
  const int32_t* x = xs.data();
  const int32_t* y = ys.data();

  int64_t total = 0;

  for (std::size_t i = 0; i < n; ++i) {
    const int32_t xi = x[i];
    const int32_t yi = y[i];
    for (std::size_t j = i + 1; j < n; ++j) {
      int64_t dx = static_cast<int64_t>(x[j]) - static_cast<int64_t>(xi);
      int64_t dy = static_cast<int64_t>(y[j]) - static_cast<int64_t>(yi);
      if (dx < 0) dx = -dx;
      if (dy < 0) dy = -dy;
      total += dx + dy;
    }
  }

  return total;
}
