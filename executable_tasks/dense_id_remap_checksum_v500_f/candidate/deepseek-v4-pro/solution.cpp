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
  // Precompute the values for each probe once, avoiding repeated binary searches.
  std::vector<uint32_t> mapped;
  mapped.reserve(probes.size());
  for (uint32_t probe : probes) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
    mapped.push_back(values[idx]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : mapped) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
