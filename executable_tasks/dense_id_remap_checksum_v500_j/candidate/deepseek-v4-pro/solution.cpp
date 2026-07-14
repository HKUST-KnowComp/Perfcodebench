#include "interface.h"

#include <algorithm>

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

  // Precompute the index mapping for each probe
  std::vector<std::size_t> indices;
  indices.reserve(probes.size());
  for (uint32_t probe : probes) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    indices.push_back(static_cast<std::size_t>(it - keys.begin()));
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // reset seed
    for (std::size_t idx : indices) {
      hash = mix(hash, values[idx]);
    }
  }
  return hash;
}