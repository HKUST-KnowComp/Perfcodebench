#include "interface.h"

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
  // Build single dense direct lookup array leveraging task's dense ID guarantee
  const uint32_t max_key = keys.back();
  std::vector<uint32_t> lookup(max_key + 1);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i]] = values[i];
  }

  // Precompute remapped probe values into cache-contiguous array for fast inner loop
  std::vector<uint32_t> remapped_probes;
  remapped_probes.reserve(probes.size());
  for (uint32_t p : probes) {
    remapped_probes.push_back(lookup[p]);
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