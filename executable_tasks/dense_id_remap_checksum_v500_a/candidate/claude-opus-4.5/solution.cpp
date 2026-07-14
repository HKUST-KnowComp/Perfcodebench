#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 1469598103934665603ULL;
  }

  // Find min and max keys to build a compact direct lookup table
  uint32_t min_key = keys[0];
  uint32_t max_key = keys[0];
  for (size_t i = 1; i < keys.size(); ++i) {
    if (keys[i] < min_key) min_key = keys[i];
    if (keys[i] > max_key) max_key = keys[i];
  }

  const uint64_t range = static_cast<uint64_t>(max_key) - min_key + 1;
  
  // If range is reasonable, use direct lookup table
  // Otherwise fall back to binary search
  constexpr uint64_t MAX_TABLE_SIZE = 100000000ULL; // 100M entries max (~400MB)
  
  if (range <= MAX_TABLE_SIZE) {
    // Build direct lookup table
    std::vector<uint32_t> lookup(static_cast<size_t>(range));
    for (size_t i = 0; i < keys.size(); ++i) {
      lookup[keys[i] - min_key] = values[i];
    }

    // Pre-remap all probes to values
    const size_t n = probes.size();
    std::vector<uint32_t> remapped(n);
    for (size_t i = 0; i < n; ++i) {
      remapped[i] = lookup[probes[i] - min_key];
    }

    // Compute hash over remapped values
    uint64_t hash = 0;
    const uint32_t* __restrict rptr = remapped.data();
    
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      size_t i = 0;
      
      // Unroll by 4 for better ILP
      const size_t n4 = n & ~size_t(3);
      for (; i < n4; i += 4) {
        hash = mix(hash, rptr[i]);
        hash = mix(hash, rptr[i + 1]);
        hash = mix(hash, rptr[i + 2]);
        hash = mix(hash, rptr[i + 3]);
      }
      
      // Handle remainder
      for (; i < n; ++i) {
        hash = mix(hash, rptr[i]);
      }
    }
    return hash;
  }
  
  // Fallback: use binary search but pre-compute indices
  const size_t n = probes.size();
  std::vector<uint32_t> remapped(n);
  for (size_t i = 0; i < n; ++i) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
    size_t idx = static_cast<size_t>(it - keys.begin());
    remapped[i] = values[idx];
  }

  uint64_t hash = 0;
  const uint32_t* __restrict rptr = remapped.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, rptr[i]);
    }
  }
  return hash;
}