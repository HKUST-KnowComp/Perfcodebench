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
    
  std::vector<uint32_t> offsets(segments + 1, 0);
  std::vector<uint64_t> sums(segments, 0);

  uint32_t current_seg = 0;
  uint64_t current_sum = 0;
  const size_t n = segment_ids.size();
  
  for (size_t i = 0; i < n; ++i) {
    uint32_t seg = segment_ids[i];
    while (current_seg < seg) {
      offsets[current_seg + 1] = static_cast<uint32_t>(i);
      sums[current_seg] = current_sum;
      current_sum = 0;
      ++current_seg;
    }
    current_sum += values[i];
  }
  while (current_seg < segments) {
    offsets[current_seg + 1] = static_cast<uint32_t>(n);
    sums[current_seg] = current_sum;
    current_sum = 0;
    ++current_seg;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    for (uint64_t v : sums) {
      hash = mix(hash, v);
    }
  }
  return hash;
}