#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  // Match baseline semantics when iters == 0
  if (iters <= 0) return 0;

  const std::size_t n = xs.size();
  if (n < 2) return 0; // No pairs

  // Compute sum of absolute differences for each coordinate separately
  int64_t total = 0;

  // Reuse one buffer to avoid extra allocations
  std::vector<int32_t> buf;
  buf.reserve(n);

  // X dimension
  buf.assign(xs.begin(), xs.end());
  std::sort(buf.begin(), buf.end());
  int64_t prefix = 0;
  for (std::size_t i = 0; i < n; ++i) {
    int64_t v = static_cast<int64_t>(buf[i]);
    total += v * static_cast<int64_t>(i) - prefix;
    prefix += v;
  }

  // Y dimension
  buf.assign(ys.begin(), ys.end());
  std::sort(buf.begin(), buf.end());
  prefix = 0;
  for (std::size_t i = 0; i < n; ++i) {
    int64_t v = static_cast<int64_t>(buf[i]);
    total += v * static_cast<int64_t>(i) - prefix;
    prefix += v;
  }

  return total;
}
