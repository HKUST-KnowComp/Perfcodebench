#include "interface.h"

#include <vector>

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& values) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : values) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t window_sum_checksum(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  if (n == 0) {
    return 0;
  }
  
  std::vector<uint64_t> out(n);
  uint64_t hash = 0;
  const std::size_t w = static_cast<std::size_t>(window);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    
    // Build initial partial sums for indices 0 to min(w-1, n-1)
    const std::size_t initial_end = (w < n) ? w : n;
    for (std::size_t i = 0; i < initial_end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
      out[i] = sum;
    }
    
    // Sliding window for remaining elements
    for (std::size_t i = initial_end; i < n; ++i) {
      sum += static_cast<uint64_t>(values[i]) - static_cast<uint64_t>(values[i - w]);
      out[i] = sum;
    }
    
    hash = checksum_u64(out);
  }
  
  return hash;
}