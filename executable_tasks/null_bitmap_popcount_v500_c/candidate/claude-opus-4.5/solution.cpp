#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  const uint64_t* data = words.data();
  const std::size_t n = words.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    std::size_t i = 0;
    
    // Unroll loop for better throughput
    for (; i + 4 <= n; i += 4) {
      count += static_cast<uint64_t>(__builtin_popcountll(data[i]));
      count += static_cast<uint64_t>(__builtin_popcountll(data[i + 1]));
      count += static_cast<uint64_t>(__builtin_popcountll(data[i + 2]));
      count += static_cast<uint64_t>(__builtin_popcountll(data[i + 3]));
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      count += static_cast<uint64_t>(__builtin_popcountll(data[i]));
    }
  }
  
  return count;
}