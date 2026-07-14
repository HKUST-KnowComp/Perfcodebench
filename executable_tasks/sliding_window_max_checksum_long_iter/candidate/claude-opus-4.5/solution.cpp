#include "interface.h"
#include <cstdint>
#include <vector>

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
  const std::size_t num_windows = n - w + 1;
  
  // Monotonic deque storing indices
  std::vector<std::size_t> deque(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t front = 0;
    std::size_t back = 0;
    
    // Initialize deque with first window-1 elements
    for (std::size_t i = 0; i < w - 1; ++i) {
      uint32_t val = data[i];
      while (back > front && data[deque[back - 1]] <= val) {
        --back;
      }
      deque[back++] = i;
    }
    
    // Process each window
    for (std::size_t i = w - 1; i < n; ++i) {
      uint32_t val = data[i];
      
      // Remove elements smaller than current from back
      while (back > front && data[deque[back - 1]] <= val) {
        --back;
      }
      deque[back++] = i;
      
      // Remove elements outside window from front
      std::size_t window_start = i - w + 1;
      while (deque[front] < window_start) {
        ++front;
      }
      
      // Maximum is at front of deque
      hash = mix(hash, data[deque[front]]);
    }
  }
  
  return hash;
}