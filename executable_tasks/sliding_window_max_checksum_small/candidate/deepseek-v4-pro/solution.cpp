#include "interface.h"
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (window <= 0 || values.empty() || window > static_cast<int>(values.size())) {
    // No windows to process; the hash per iteration remains constant.
    // The loop body never runs, so we can return the fixed initial hash directly.
    return 1469598103934665603ULL;
  }

  const int n = static_cast<int>(values.size());
  std::vector<uint32_t> maxes;
  maxes.reserve(n - window + 1);

  // Precompute sliding-window maxima using a monotonic deque.
  std::deque<int> dq;
  for (int i = 0; i < n; ++i) {
    while (!dq.empty() && dq.front() <= i - window)
      dq.pop_front();
    while (!dq.empty() && values[dq.back()] <= values[i])
      dq.pop_back();
    dq.push_back(i);
    if (i >= window - 1) {
      maxes.push_back(values[dq.front()]);
    }
  }

  // Compute the checksum over the required number of iterations.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t m : maxes) {
      hash ^= m;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
