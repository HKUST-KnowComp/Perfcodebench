#include "interface.h"

#include <vector>
#include <numeric>

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
  std::vector<uint32_t> segment_sizes(static_cast<std::size_t>(segments), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(segment_sizes.begin(), segment_sizes.end(), 0);
    for (uint32_t id : segment_ids) {
      segment_sizes[static_cast<std::size_t>(id)] += 1;
    }
    std::partial_sum(segment_sizes.begin(), segment_sizes.end(), offsets.begin() + 1);
    for (std::size_t i = 0; i < segment_ids.size(); ++i) {
      sums[static_cast<std::size_t>(segment_ids[i])] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (uint32_t value : offsets) {
      hash = mix(hash, value);
    }
    for (uint64_t value : sums) {
      hash = mix(hash, value);
    }
    std::fill(sums.begin(), sums.end(), 0);
  }
  return hash;
}
