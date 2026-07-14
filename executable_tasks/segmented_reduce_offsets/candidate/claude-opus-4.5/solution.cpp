#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segmented_reduce_checksum(
    const std::vector<uint32_t>& segment_ids,
    const std::vector<uint32_t>& values,
    uint32_t segments,
    int iters) {
  const std::size_t n = segment_ids.size();
  const std::size_t num_segments = static_cast<std::size_t>(segments);
  
  // Pre-allocate once outside the loop
  std::vector<uint32_t> offsets(num_segments + 1);
  std::vector<uint64_t> sums(num_segments);
  
  uint64_t hash = 0;
  
  // Get raw pointers for faster access
  const uint32_t* __restrict__ seg_ptr = segment_ids.data();
  const uint32_t* __restrict__ val_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset arrays
    for (std::size_t i = 0; i <= num_segments; ++i) {
      offsets[i] = 0;
    }
    for (std::size_t i = 0; i < num_segments; ++i) {
      sums[i] = 0;
    }
    
    // Since segment_ids are sorted, we can compute counts and sums in one pass
    // First pass: count elements per segment and compute sums
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t seg = seg_ptr[i];
      offsets[seg + 1]++;
      sums[seg] += static_cast<uint64_t>(val_ptr[i]);
    }
    
    // Convert counts to offsets (prefix sum)
    for (std::size_t s = 1; s <= num_segments; ++s) {
      offsets[s] += offsets[s - 1];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i <= num_segments; ++i) {
      hash = mix(hash, offsets[i]);
    }
    for (std::size_t i = 0; i < num_segments; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  
  return hash;
}