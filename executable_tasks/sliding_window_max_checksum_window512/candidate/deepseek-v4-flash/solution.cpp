#include "interface.h"
#include <cstdint>
#include <vector>
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Precompute all sliding window maxima using a monotonic deque.
  std::vector<uint32_t> maxima;
  if (window > 0 && static_cast<std::size_t>(window) <= values.size()) {
    std::size_t n = values.size();
    std::size_t w = static_cast<std::size_t>(window);
    std::deque<std::size_t> dq;
    maxima.reserve(n - w + 1);
    for (std::size_t i = 0; i < n; ++i) {
      // Remove elements from back that are not greater than current
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      dq.push_back(i);
      // Remove elements from front that are out of the window
      if (dq.front() + w <= i) {
        dq.pop_front();
      }
      // If we have a full window, record the maximum
      if (i >= w - 1) {
        maxima.push_back(values[dq.front()]);
      }
    }
  }

  // Iterate over the precomputed maxima to compute the hash.
  uint64_t hash = 0;
  const uint64_t seed = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = seed;
    for (std::size_t idx = 0; idx < maxima.size(); ++idx) {
      hash = mix(hash, maxima[idx]);
    }
  }
  return hash;
}
