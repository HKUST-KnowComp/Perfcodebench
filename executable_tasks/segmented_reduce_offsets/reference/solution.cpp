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
  std::vector<uint32_t> offsets(static_cast<std::size_t>(segments) + 1, 0);
  std::vector<uint64_t> sums(static_cast<std::size_t>(segments), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0);
    std::fill(sums.begin(), sums.end(), 0);
    for (std::size_t i = 0; i < segment_ids.size(); ++i) {
      ++offsets[static_cast<std::size_t>(segment_ids[i] + 1)];
      sums[static_cast<std::size_t>(segment_ids[i])] += static_cast<uint64_t>(values[i]);
    }
    for (uint32_t s = 0; s < segments; ++s) {
      offsets[static_cast<std::size_t>(s + 1)] += offsets[static_cast<std::size_t>(s)];
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
