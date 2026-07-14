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
  if (values.empty()) return 0;
  
  std::vector<uint64_t> out(values.size(), 0);
  uint64_t hash = 0;
  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Initialize first window sum
    uint64_t sum = 0;
    std::size_t first_window_size = (n < w) ? n : w;
    for (std::size_t j = 0; j < first_window_size; ++j) {
      sum += static_cast<uint64_t>(values[j]);
    }
    out[0] = static_cast<uint64_t>(values[0]);
    
    // Compute remaining positions using sliding window
    for (std::size_t i = 1; i < n; ++i) {
      sum += static_cast<uint64_t>(values[i]);
      if (i >= w) {
        sum -= static_cast<uint64_t>(values[i - w]);
      }
      out[i] = sum;
    }
    
    hash = checksum_u64(out);
  }
  return hash;
}
