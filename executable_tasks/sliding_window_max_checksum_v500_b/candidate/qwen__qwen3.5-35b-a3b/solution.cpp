#include "interface.h"
#include <deque>
#include <vector>

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
    
    // Monotonic deque for O(n) sliding window maximum
    std::deque<std::size_t> dq;
    
    for (std::size_t i = 0; i < values.size(); ++i) {
      // Remove indices outside the current window
      while (!dq.empty() && dq.front() <= i - static_cast<std::size_t>(window)) {
        dq.pop_front();
      }
      
      // Remove indices with values smaller than current (maintain decreasing order)
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      
      // Add current index
      dq.push_back(i);
      
      // Window is complete; extract maximum from front of deque
      if (i >= static_cast<std::size_t>(window) - 1) {
        uint32_t mx = values[dq.front()];
        hash = mix(hash, mx);
      }
    }
  }
  return hash;
}