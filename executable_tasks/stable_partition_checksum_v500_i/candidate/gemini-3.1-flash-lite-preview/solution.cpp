#include "interface.h"
#include <vector>
#include <algorithm>

namespace {
inline uint64_t mix(uint64_t hash, uint32_t value) {
  hash ^= static_cast<uint64_t>(value);
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags, 
    const std::vector<uint32_t>& values, 
    int iters) {
  const size_t n = values.size();
  std::vector<uint32_t> out(n);
  std::vector<uint32_t> left(n);
  std::vector<uint32_t> right(n);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t l_idx = 0;
    size_t r_idx = 0;
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        left[l_idx++] = values[i];
      } else {
        right[r_idx++] = values[i];
      }
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < l_idx; ++i) {
      hash = mix(hash, left[i]);
    }
    for (size_t i = 0; i < r_idx; ++i) {
      hash = mix(hash, right[i]);
    }
  }
  return hash;
}