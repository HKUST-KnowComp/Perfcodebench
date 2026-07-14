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
  std::vector<std::vector<uint32_t>> buckets;
  std::vector<uint32_t> offsets;
  std::vector<uint64_t> sums;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    buckets.assign(static_cast<std::size_t>(segments), {});
    for (std::size_t i = 0; i < segment_ids.size(); ++i) {
      buckets[static_cast<std::size_t>(segment_ids[i])].push_back(values[i]);
    }
    offsets.assign(static_cast<std::size_t>(segments) + 1, 0);
    sums.assign(static_cast<std::size_t>(segments), 0);
    for (uint32_t s = 0; s < segments; ++s) {
      offsets[static_cast<std::size_t>(s + 1)] =
          offsets[static_cast<std::size_t>(s)] + static_cast<uint32_t>(buckets[static_cast<std::size_t>(s)].size());
      for (uint32_t value : buckets[static_cast<std::size_t>(s)]) {
        sums[static_cast<std::size_t>(s)] += static_cast<uint64_t>(value);
      }
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
