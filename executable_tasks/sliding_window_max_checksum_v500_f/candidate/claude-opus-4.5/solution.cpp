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
  
  const uint32_t* data = values.data();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::deque<std::size_t> dq;
    
    // Initialize deque with first window-1 elements
    for (std::size_t i = 0; i < w - 1; ++i) {
      while (!dq.empty() && data[dq.back()] <= data[i]) {
        dq.pop_back();
      }
      dq.push_back(i);
    }
    
    // Process each window
    for (std::size_t i = w - 1; i < n; ++i) {
      // Remove elements outside current window
      while (!dq.empty() && dq.front() <= i - w) {
        dq.pop_front();
      }
      
      // Remove elements smaller than current from back
      while (!dq.empty() && data[dq.back()] <= data[i]) {
        dq.pop_back();
      }
      dq.push_back(i);
      
      // Front of deque is the maximum
      hash = mix(hash, data[dq.front()]);
    }
  }
  
  return hash;
}