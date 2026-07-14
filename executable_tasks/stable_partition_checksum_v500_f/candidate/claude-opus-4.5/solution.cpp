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
  
  // Pre-compute prefix sums for left indices and right indices
  // left_prefix[i] = number of flags[j]==1 for j < i
  // right_prefix[i] = number of flags[j]==0 for j < i
  std::vector<uint32_t> left_prefix(n + 1);
  std::vector<uint32_t> right_prefix(n + 1);
  
  const uint8_t* __restrict fp = flags.data();
  const uint32_t* __restrict vp = values.data();
  uint32_t* __restrict op = out.data();
  
  // Compute total count of 1s (left partition size)
  uint32_t left_count = 0;
  for (size_t i = 0; i < n; ++i) {
    left_count += fp[i];
  }
  
  const uint32_t right_start = left_count;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Build prefix sums
    uint32_t lp = 0;
    uint32_t rp = 0;
    
    // Single pass: scatter values directly to output positions
    // For flag=1: position = left_prefix[i]
    // For flag=0: position = right_start + right_prefix[i]
    uint32_t left_idx = 0;
    uint32_t right_idx = right_start;
    
    for (size_t i = 0; i < n; ++i) {
      if (fp[i]) {
        op[left_idx++] = vp[i];
      } else {
        op[right_idx++] = vp[i];
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    
    // Unroll hash computation for better performance
    size_t i = 0;
    const size_t unroll_end = n - (n % 4);
    for (; i < unroll_end; i += 4) {
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