#include "interface.h"

#include <vector>
#include <cstdint>

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
  const std::size_t n = segment_ids.size();
  std::vector<uint32_t> counts(segments, 0);
  std::vector<uint64_t> sums(segments, 0);

  // Single pass over all data to compute counts and sums per segment.
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t seg = segment_ids[i];
    counts[seg] += 1;
    sums[seg] += values[i];
  }

  // Precompute offset array (cumulative counts).
  std::vector<uint32_t> offsets(segments + 1, 0);
  uint32_t cum = 0;
  for (uint32_t s = 0; s < segments; ++s) {
    offsets[s] = cum;
    cum += counts[s];
  }
  offsets[segments] = cum;

  // Hash mixing is repeated for each iteration using the precomputed arrays.
  uint64_t hash = 0;
  const uint64_t seed = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = seed;
    for (uint32_t val : offsets) {
      hash = mix(hash, val);
    }
    for (uint64_t val : sums) {
      hash = mix(hash, val);
    }
  }
  return hash;
}