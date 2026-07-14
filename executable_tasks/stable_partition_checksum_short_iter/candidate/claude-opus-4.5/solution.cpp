#include "interface.h"

#include <vector>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }
  
  // Pre-allocate output buffer once
  std::vector<uint32_t> out(n);
  
  const uint8_t* __restrict fp = flags.data();
  const uint32_t* __restrict vp = values.data();
  uint32_t* __restrict op = out.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Count elements going to left partition
    std::size_t left_count = 0;
    for (std::size_t i = 0; i < n; ++i) {
      left_count += fp[i] ? 1 : 0;
    }
    
    // Scatter values to their final positions
    std::size_t left_idx = 0;
    std::size_t right_idx = left_count;
    
    for (std::size_t i = 0; i < n; ++i) {
      if (fp[i]) {
        op[left_idx++] = vp[i];
      } else {
        op[right_idx++] = vp[i];
      }
    }
    
    // Compute hash over partitioned output
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, op[i]);
    }
  }
  
  return hash;
}
