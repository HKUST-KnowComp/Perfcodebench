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
  const size_t n = segment_ids.size();
  std::vector<uint32_t> offsets(segments + 1, 0);
  std::vector<uint64_t> sums(segments, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0);
    std::fill(sums.begin(), sums.end(), 0);

    for (size_t i = 0; i < n; ++i) {
      uint32_t id = segment_ids[i];
      offsets[id + 1]++;
      sums[id] += values[i];
    }

    for (uint32_t i = 0; i < segments; ++i) {
      offsets[i + 1] += offsets[i];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t val : offsets) {
      hash = mix(hash, val);
    }
    for (uint64_t val : sums) {
      hash = mix(hash, val);
    }
  }
  return hash;
}