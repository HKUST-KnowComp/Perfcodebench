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
  const std::size_t w = static_cast<std::size_t>(window);
  
  if (n < w) return hash;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::deque<std::size_t> dq;
    
    // Initialize first window
    for (std::size_t j = 0; j < w; ++j) {
      while (!dq.empty() && values[dq.back()] <= values[j]) {
        dq.pop_back();
      }
      dq.push_back(j);
    }
    hash = mix(hash, values[dq.front()]);
    
    // Slide window
    for (std::size_t i = w; i < n; ++i) {
      // Remove elements outside window
      while (!dq.empty() && dq.front() <= i - w) {
        dq.pop_front();
      }
      
      // Remove elements smaller than current
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      
      dq.push_back(i);
      hash = mix(hash, values[dq.front()]);
    }
  }
  
  return hash;
}