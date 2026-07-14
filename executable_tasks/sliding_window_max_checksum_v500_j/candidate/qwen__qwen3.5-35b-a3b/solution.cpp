#include "interface.h"
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::deque<int> dq;
    for (std::size_t i = 0; i < values.size(); ++i) {
      // Remove indices that are out of the current window
      while (!dq.empty() && dq.front() < static_cast<int>(i) - window + 1) {
        dq.pop_front();
      }
      
      // Remove indices from back whose values are smaller than current
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      
      // Add current index
      dq.push_back(i);
      
      // Once we have a complete window, record the maximum
      if (i >= static_cast<std::size_t>(window) - 1) {
        uint32_t mx = values[dq.front()];
        hash = mix(hash, mx);
      }
    }
  }
  return hash;
}