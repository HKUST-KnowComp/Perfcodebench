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
  
  // Pre-compute prefix sums for true flags (positions in left partition)
  // and false flags (positions in right partition)
  std::vector<uint32_t> true_prefix(n);
  std::vector<uint32_t> false_prefix(n);
  
  const uint8_t* __restrict fp = flags.data();
  const uint32_t* __restrict vp = values.data();
  uint32_t* __restrict op = out.data();
  
  // Count total true flags
  size_t true_count = 0;
  for (size_t i = 0; i < n; ++i) {
    true_count += fp[i] ? 1 : 0;
  }
  
  // Compute prefix sums
  uint32_t t_idx = 0;
  uint32_t f_idx = 0;
  for (size_t i = 0; i < n; ++i) {
    if (fp[i]) {
      true_prefix[i] = t_idx++;
    } else {
      false_prefix[i] = f_idx++;
    }
  }
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Scatter values to their final positions
    for (size_t i = 0; i < n; ++i) {
      if (fp[i]) {
        op[true_prefix[i]] = vp[i];
      } else {
        op[true_count + false_prefix[i]] = vp[i];
      }
    }
    
    // Compute hash with unrolling
    hash = 1469598103934665603ULL;
    size_t i = 0;
    size_t n4 = n & ~3ULL;
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