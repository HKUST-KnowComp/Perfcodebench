#include "interface.h"
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  uint64_t hash = 0;
  const std::size_t w = static_cast<std::size_t>(window);
  if (values.empty() || w > values.size()) {
    return hash;
  }
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::deque<std::size_t> dq;
    dq.reserve(w);
    // Process first window
    for (std::size_t j = 0; j < w; ++j) {
      while (!dq.empty() && values[j] >= values[dq.back()]) {
        dq.pop_back();
      }
      dq.push_back(j);
    }
    hash = mix(hash, values[dq.front()]);
    // Process remaining elements to generate all subsequent window maxima
    for (std::size_t j = w; j < values.size(); ++j) {
      // Remove out-of-window indices from front
      while (!dq.empty() && dq.front() <= j - w) {
        dq.pop_front();
      }
      // Maintain monotonic decreasing order by removing smaller elements from back
      while (!dq.empty() && values[j] >= values[dq.back()]) {
        dq.pop_back();
      }
      dq.push_back(j);
      // Add current window's maximum to hash
      hash = mix(hash, values[dq.front()]);
    }
  }
  return hash;
}