#include "interface.h"
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);
  
  if (n < w || w == 0) {
    return 0;
  }
  
  uint64_t hash = 0;
  const uint32_t* data = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Monotonic deque storing indices; values at these indices are in decreasing order
    std::deque<std::size_t> dq;
    
    // Process first window
    for (std::size_t i = 0; i < w; ++i) {
      while (!dq.empty() && data[dq.back()] <= data[i]) {
        dq.pop_back();
      }
      dq.push_back(i);
    }
    hash = mix(hash, data[dq.front()]);
    
    // Slide the window
    const std::size_t num_windows = n - w + 1;
    for (std::size_t i = 1; i < num_windows; ++i) {
      // Remove elements outside the window from front
      if (dq.front() < i) {
        dq.pop_front();
      }
      
      // Add new element, removing smaller elements from back
      std::size_t new_idx = i + w - 1;
      uint32_t new_val = data[new_idx];
      while (!dq.empty() && data[dq.back()] <= new_val) {
        dq.pop_back();
      }
      dq.push_back(new_idx);
      
      hash = mix(hash, data[dq.front()]);
    }
  }
  
  return hash;
}