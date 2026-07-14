#include "interface.h"
#include <deque>
#include <cstddef>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Precompute sliding window maxima
  std::size_t n = values.size();
  std::size_t w = static_cast<std::size_t>(window);
  std::vector<uint32_t> maxes;
  if (n >= w) {
    maxes.reserve(n - w + 1);
    std::deque<std::size_t> dq; // indices of potential maxima
    for (std::size_t i = 0; i < n; ++i) {
      // Remove indices out of window
      while (!dq.empty() && dq.front() <= i - w) {
        dq.pop_front();
      }
      // Remove smaller values from the back
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      dq.push_back(i);
      // After the first full window, record the maximum
      if (i >= w - 1) {
        maxes.push_back(values[dq.front()]);
      }
    }
  }
  // Hashing loop
  const uint64_t base = 1469598103934665603ULL;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = base;
    for (std::size_t i = 0; i < maxes.size(); ++i) {
      hash = mix(hash, maxes[i]);
    }
  }
  return hash;
}