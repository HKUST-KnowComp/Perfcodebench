#include "interface.h"
#include <deque>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0 || static_cast<size_t>(window) > values.size()) {
    return 0;
  }

  const size_t n = values.size();
  const size_t w = static_cast<size_t>(window);
  
  // Pre-calculate all sliding window maxima once to avoid O(iters * N * window) complexity.
  // We use a monotonic queue (deque) to achieve O(N) total time for this pre-calculation.
  std::vector<uint32_t> maxes;
  maxes.reserve(n - w + 1);

  std::deque<size_t> dq;
  for (size_t i = 0; i < n; ++i) {
    // Remove indices that are out of the current window
    if (!dq.empty() && dq.front() <= i - w) {
      dq.pop_front();
    }

    // Remove elements smaller than the current element from the back
    while (!dq.empty() && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }

    dq.push_back(i);

    // The front of the deque is the index of the maximum element for the current window
    if (i >= w - 1) {
      maxes.push_back(values[dq.front()]);
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t mx : maxes) {
      hash = mix(hash, static_cast<uint64_t>(mx));
    }
  }

  return hash;
}
