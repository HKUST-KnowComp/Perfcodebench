#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

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
  
  // Pre-compute prefix sums for flags (count of 1s up to each position)
  // and inverse (count of 0s up to each position)
  std::vector<uint32_t> prefix_ones(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Compute prefix sum of flags (number of 1s before each position)
    uint32_t count_ones = 0;
    
#if defined(__AVX2__)
    // Process in chunks for better cache utilization
    std::size_t i = 0;
    
    // Scalar prefix sum - needed for correct indices
    for (; i < n; ++i) {
      prefix_ones[i] = count_ones;
      count_ones += flags[i];
    }
#else
    for (std::size_t i = 0; i < n; ++i) {
      prefix_ones[i] = count_ones;
      count_ones += flags[i];
    }
#endif
    
    const uint32_t total_ones = count_ones;
    
    // Scatter values to their final positions
    // If flag[i] == 1: position = prefix_ones[i] (goes to left partition)
    // If flag[i] == 0: position = total_ones + (i - prefix_ones[i]) (goes to right partition)
    
    const uint8_t* __restrict f = flags.data();
    const uint32_t* __restrict v = values.data();
    const uint32_t* __restrict p = prefix_ones.data();
    uint32_t* __restrict o = out.data();
    
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t pos;
      if (f[i]) {
        pos = p[i];
      } else {
        pos = total_ones + (static_cast<uint32_t>(i) - p[i]);
      }
      o[pos] = v[i];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    
#if defined(__AVX2__)
    // Unroll hash computation for better performance
    std::size_t j = 0;
    const std::size_t unroll = 8;
    const std::size_t limit = n - (n % unroll);
    
    for (; j < limit; j += unroll) {
      hash = mix(hash, o[j]);
      hash = mix(hash, o[j+1]);
      hash = mix(hash, o[j+2]);
      hash = mix(hash, o[j+3]);
      hash = mix(hash, o[j+4]);
      hash = mix(hash, o[j+5]);
      hash = mix(hash, o[j+6]);
      hash = mix(hash, o[j+7]);
    }
    
    for (; j < n; ++j) {
      hash = mix(hash, o[j]);
    }
#else
    for (std::size_t j = 0; j < n; ++j) {
      hash = mix(hash, o[j]);
    }
#endif
  }
  
  return hash;
}