#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint32_t> offsets;
  std::vector<uint64_t> sums;
  uint64_t hash = 0;
  
  offsets.resize(static_cast<std::size_t>(segments) + 1);
  sums.resize(static_cast<std::size_t>(segments));
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets and sums
    for (uint32_t s = 0; s <= segments; ++s) {
      offsets[s] = 0;
    }
    for (uint32_t s = 0; s < segments; ++s) {
      sums[s] = 0;
    }
    
    // Single pass: exploit sorted segment_ids
    std::size_t n = segment_ids.size();
    if (n > 0) {
      uint32_t current_seg = segment_ids[0];
      uint32_t count = 0;
      uint64_t sum = 0;
      
      for (std::size_t i = 0; i < n; ++i) {
        uint32_t seg = segment_ids[i];
        if (seg != current_seg) {
          // Finalize previous segment
          offsets[current_seg + 1] = count;
          sums[current_seg] = sum;
          current_seg = seg;
          count = 0;
          sum = 0;
        }
        count++;
        sum += static_cast<uint64_t>(values[i]);
      }
      // Finalize last segment
      offsets[current_seg + 1] = count;
      sums[current_seg] = sum;
      
      // Convert counts to cumulative offsets
      uint32_t cumulative = 0;
      for (uint32_t s = 0; s <= segments; ++s) {
        uint32_t temp = offsets[s];
        offsets[s] = cumulative;
        cumulative += temp;
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (uint32_t s = 0; s <= segments; ++s) {
      hash = mix(hash, offsets[s]);
    }
    for (uint32_t s = 0; s < segments; ++s) {
      hash = mix(hash, sums[s]);
    }
  }
  return hash;
}