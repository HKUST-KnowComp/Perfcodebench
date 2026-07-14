#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  // Precompute remapped indices once to avoid O(log N) binary search per probe per iteration
  std::vector<uint32_t> indices(probes.size());
  for (size_t i = 0; i < probes.size(); ++i) {
    indices[i] = static_cast<uint32_t>(std::lower_bound(keys.begin(), keys.end(), probes[i]) - keys.begin());
  }

  uint64_t hash = 0;
  const uint32_t* values_data = values.data();
  const uint32_t* indices_data = indices.data();
  const size_t n = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, values_data[indices_data[i]]);
    }
  }
  return hash;
}
