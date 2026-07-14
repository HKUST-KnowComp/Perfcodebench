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
  const size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Pre-allocate output buffer once
  std::vector<uint32_t> out(n);
  
  // Precompute prefix sums for left indices and right indices
  // left_prefix[i] = number of flags[j]==1 for j < i
  // right_prefix[i] = number of flags[j]==0 for j < i
  std::vector<uint32_t> left_prefix(n + 1);
  std::vector<uint32_t> right_prefix(n + 1);
  
  left_prefix[0] = 0;
  right_prefix[0] = 0;
  
  for (size_t i = 0; i < n; ++i) {
    uint32_t f = flags[i];
    left_prefix[i + 1] = left_prefix[i] + f;
    right_prefix[i + 1] = right_prefix[i] + (1 - f);
  }
  
  const uint32_t total_left = left_prefix[n];
  
  uint64_t hash = 0;
  
  const uint8_t* __restrict fp = flags.data();
  const uint32_t* __restrict vp = values.data();
  uint32_t* __restrict op = out.data();
  const uint32_t* __restrict lp = left_prefix.data();
  const uint32_t* __restrict rp = right_prefix.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Scatter values to their final positions in one pass
    for (size_t i = 0; i < n; ++i) {
      uint32_t idx = fp[i] ? lp[i] : (total_left + rp[i]);
      op[idx] = vp[i];
    }
    
    // Compute hash with unrolling
    hash = 1469598103934665603ULL;
    size_t i = 0;
    size_t n4 = n & ~size_t(3);
    for (; i < n4; i += 4) {
      hash = mix(hash, op[i]);
      hash = mix(hash, op[i + 1]);
      hash = mix(hash, op[i + 2]);
      hash = mix(hash, op[i + 3]);
    }
    for (; i < n; ++i) {
      hash = mix(hash, op[i]);
    }
  }
  
  return hash;
}