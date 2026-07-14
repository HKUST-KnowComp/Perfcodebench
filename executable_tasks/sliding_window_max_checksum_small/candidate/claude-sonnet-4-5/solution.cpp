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
  const std::size_t n = values.size();
  const std::size_t num_windows = n >= static_cast<std::size_t>(window) ? n - static_cast<std::size_t>(window) + 1 : 0;
  
  if (num_windows == 0) {
    return 1469598103934665603ULL;
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::deque<std::size_t> dq;
    
    // Initialize first window
    for (int j = 0; j < window; ++j) {
      while (!dq.empty() && values[dq.back()] <= values[j]) {
        dq.pop_back();
      }
      dq.push_back(j);
    }
    hash = mix(hash, values[dq.front()]);
    
    // Slide the window
    for (std::size_t i = 1; i < num_windows; ++i) {
      std::size_t new_idx = i + static_cast<std::size_t>(window) - 1;
      
      // Remove elements outside current window
      while (!dq.empty() && dq.front() < i) {
        dq.pop_front();
      }
      
      // Add new element
      while (!dq.empty() && values[dq.back()] <= values[new_idx]) {
        dq.pop_back();
      }
      dq.push_back(new_idx);
      
      hash = mix(hash, values[dq.front()]);
    }
  }
  return hash;
}