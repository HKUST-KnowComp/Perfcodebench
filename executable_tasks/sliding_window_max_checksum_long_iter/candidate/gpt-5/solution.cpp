#include "interface.h"
#include <deque>
#include <cstddef>

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Preserve baseline behavior: if iters <= 0, the loop never runs and hash remains 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  const std::size_t n = values.size();

  // If window invalid relative to data size, baseline would reset hash to offset and do no mixing
  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    return FNV_OFFSET;
  }

  // Fast path: window == 1 -> maxima are the elements themselves
  if (window == 1) {
    uint64_t hash = FNV_OFFSET;
    for (std::size_t i = 0; i < n; ++i) {
      hash ^= static_cast<uint64_t>(values[i]);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  // General case: use a monotonic deque to compute sliding window maxima in O(n)
  uint64_t hash = FNV_OFFSET;
  std::deque<std::size_t> dq; // stores indices, values are monotonic decreasing
  const std::size_t w = static_cast<std::size_t>(window);

  for (std::size_t i = 0; i < n; ++i) {
    // Remove elements smaller than or equal to current from the back
    while (!dq.empty() && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }
    dq.push_back(i);

    // Remove elements outside the current window from the front
    if (!dq.empty() && dq.front() + w <= i) {
      dq.pop_front();
    }

    // Once the first window is formed, mix the current maximum
    if (i + 1 >= w) {
      uint32_t mx = values[dq.front()];
      hash ^= static_cast<uint64_t>(mx);
      hash *= FNV_PRIME;
    }
  }

  return hash;
}
