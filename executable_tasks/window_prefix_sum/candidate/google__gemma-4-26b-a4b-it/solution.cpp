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
  if (n == 0) return 0;
  
  std::vector<uint64_t> out(n);
  const std::size_t w = static_cast<std::size_t>(window);
  uint64_t hash = 0;

  // Pre-calculate the first window sum to initialize the sliding window
  // However, since the window size effectively grows from 1 to 'window' at the start,
  // we can just use a single pass sliding window logic.

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      current_sum += values[i];
      if (i >= w) {
        current_sum -= values[i - w];
      }
      out[i] = current_sum;
    }
    hash = checksum_u64(out);
  }

  return hash;
}