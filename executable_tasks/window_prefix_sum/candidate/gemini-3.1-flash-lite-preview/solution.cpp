#include "interface.h"
#include <vector>
#include <cstdint>

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
  const size_t n = values.size();
  if (n == 0) return 0;
  
  std::vector<uint64_t> out(n);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      current_sum += static_cast<uint64_t>(values[i]);
      if (i >= static_cast<size_t>(window)) {
        current_sum -= static_cast<uint64_t>(values[i - window]);
      }
      out[i] = current_sum;
    }
    hash = checksum_u64(out);
  }
  return hash;
}