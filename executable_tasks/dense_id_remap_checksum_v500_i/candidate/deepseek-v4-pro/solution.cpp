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
  // Precompute indices for each probe once.
  std::vector<std::size_t> indices(probes.size());
  for (std::size_t i = 0; i < probes.size(); ++i) {
    const auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
    indices[i] = static_cast<std::size_t>(it - keys.begin());
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t idx : indices) {
      hash = mix(hash, values[idx]);
    }
  }
  return hash;
}