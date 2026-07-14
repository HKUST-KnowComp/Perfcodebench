#include "interface.h"

#include <vector>

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
  
  const size_t n = segment_ids.size();
  std::vector<uint32_t> offsets(segments + 1);
  std::vector<uint64_t> sums(segments);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (n == 0) {
      // Handle empty input case
      for (uint32_t s = 0; s <= segments; ++s) offsets[s] = 0;
      for (uint32_t s = 0; s < segments; ++s) sums[s] = 0;
    } else {
      uint32_t current_seg = 0;
      uint64_t current_sum = 0;
      size_t count = 0;
      size_t i = 0;

      while (current_seg < segments) {
        size_t start_idx = i;
        while (i < n && segment_ids[i] == current_seg) {
          current_sum += values[i];
          i++;
        }
        
        offsets[current_seg] = static_cast<uint32_t>(start_idx);
        sums[current_seg] = current_sum;
        
        current_seg++;
        current_sum = 0;
      }
      offsets[segments] = static_cast<uint32_t>(n);
    }

    hash = 1469598103934665603ULL;
    for (uint32_t value : offsets) {
      hash = mix(hash, value);
    }
    for (uint64_t value : sums) {
      hash = mix(hash, value);
    }
  }
  return hash;
}