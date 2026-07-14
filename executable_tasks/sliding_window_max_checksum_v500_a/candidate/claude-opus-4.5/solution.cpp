#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);
  
  if (n < w || w == 0) {
    return 0;
  }
  
  const uint32_t* __restrict__ data = values.data();
  
  // Allocate deque storage once
  std::vector<std::size_t> deque_storage(n);
  std::size_t* __restrict__ deq = deque_storage.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t front = 0;
    std::size_t back = 0;
    
    // Process first window - 1 elements
    for (std::size_t i = 0; i < w - 1; ++i) {
      const uint32_t val = data[i];
      // Remove elements smaller than current from back
      while (back > front && data[deq[back - 1]] <= val) {
        --back;
      }
      deq[back++] = i;
    }
    
    // Process remaining elements and compute hash
    const std::size_t num_windows = n - w + 1;
    for (std::size_t i = 0; i < num_windows; ++i) {
      const std::size_t new_idx = i + w - 1;
      const uint32_t val = data[new_idx];
      
      // Remove elements smaller than current from back
      while (back > front && data[deq[back - 1]] <= val) {
        --back;
      }
      deq[back++] = new_idx;
      
      // Remove elements outside window from front
      while (deq[front] < i) {
        ++front;
      }
      
      // Maximum is at front of deque
      hash = mix(hash, data[deq[front]]);
    }
  }
  
  return hash;
}