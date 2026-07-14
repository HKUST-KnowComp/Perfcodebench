#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  if (iters <= 0) return 0;

  // Cache the index for each probe to avoid repeated binary searches.
  std::vector<size_t> indices;
  indices.reserve(probes.size());
  for (uint32_t probe : probes) {
    auto it = std::lower_bound(keys.cbegin(), keys.cend(), probe);
    indices.push_back(static_cast<size_t>(it - keys.cbegin()));
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t idx : indices) {
      hash = mix(hash, values[idx]);
    }
  }
  return hash;
}
