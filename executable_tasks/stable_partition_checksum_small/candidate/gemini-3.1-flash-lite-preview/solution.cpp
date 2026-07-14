#include "interface.h"
#include <vector>
#include <cstdint>

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
  std::vector<uint32_t> right(n);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t left_idx = 0;
    size_t right_idx = 0;
    
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        out[left_idx++] = values[i];
      } else {
        right[right_idx++] = values[i];
      }
    }
    
    for (size_t i = 0; i < right_idx; ++i) {
      out[left_idx + i] = right[i];
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, out[i]);
    }
  }
  return hash;
}