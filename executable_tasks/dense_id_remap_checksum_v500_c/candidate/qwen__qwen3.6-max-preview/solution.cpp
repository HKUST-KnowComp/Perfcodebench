#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  if (keys.empty()) return 0;

  // Build direct lookup table for O(1) remapping.
  // 'Dense' keys imply the range is compact and fits efficiently in memory.
  uint32_t max_key = keys.back();
  std::vector<uint32_t> lookup(max_key + 1);
  for (std::size_t i = 0; i < keys.size(); ++i) {
      lookup[keys[i]] = values[i];
  }

  const uint32_t* __restrict__ lookup_ptr = lookup.data();
  const uint32_t* __restrict__ probes_ptr = probes.data();
  const std::size_t n_probes = probes.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n_probes; ++i) {
      hash = mix(hash, lookup_ptr[probes_ptr[i]]);
    }
  }
  return hash;
}
