#include "interface.h"

#include <vector>
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
  // Precompute remapped values once to avoid redundant binary searches across iterations
  std::vector<uint32_t> remapped_probes;
  remapped_probes.reserve(probes.size());
  for (uint32_t probe : probes) {
    const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    const size_t idx = static_cast<size_t>(it - keys.begin());
    remapped_probes.push_back(values[idx]);
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = 1469598103934665603ULL;
    for (uint32_t v : remapped_probes) {
      final_hash = mix(final_hash, v);
    }
  }
  return final_hash;
}