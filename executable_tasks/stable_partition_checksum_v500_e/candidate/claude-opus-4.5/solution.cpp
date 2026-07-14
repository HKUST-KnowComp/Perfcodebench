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
  
  // Pre-compute prefix sums for left indices
  // prefix_left[i] = number of flags[j]==1 for j < i
  std::vector<size_t> prefix_left(n + 1);
  prefix_left[0] = 0;
  
  size_t total_left = 0;
  for (size_t i = 0; i < n; ++i) {
    prefix_left[i] = total_left;
    total_left += flags[i];
  }
  prefix_left[n] = total_left;
  
  const size_t right_start = total_left;
  
  // Pre-compute right indices: for each position, how many zeros before it
  // right_idx[i] = right_start + (number of flags[j]==0 for j < i)
  std::vector<size_t> scatter_idx(n);
  
  size_t right_count = 0;
  for (size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      scatter_idx[i] = prefix_left[i];
    } else {
      scatter_idx[i] = right_start + right_count;
      right_count++;
    }
  }

  uint64_t hash = 0;
  
  const uint32_t* __restrict vptr = values.data();
  uint32_t* __restrict optr = out.data();
  const size_t* __restrict sptr = scatter_idx.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Scatter values to their final positions
    for (size_t i = 0; i < n; ++i) {
      optr[sptr[i]] = vptr[i];
    }
    
    // Compute hash with unrolling
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    const size_t unroll_end = n - (n % 4);
    
    for (; i < unroll_end; i += 4) {
      hash = mix(hash, optr[i]);
      hash = mix(hash, optr[i + 1]);
      hash = mix(hash, optr[i + 2]);
      hash = mix(hash, optr[i + 3]);
    }
    
    for (; i < n; ++i) {
      hash = mix(hash, optr[i]);
    }
  }
  
  return hash;
}