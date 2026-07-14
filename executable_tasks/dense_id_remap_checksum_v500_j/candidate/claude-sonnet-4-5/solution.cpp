#include "interface.h"

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
  uint64_t hash = 0;
  const uint32_t max_idx = static_cast<uint32_t>(values.size());
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      const uint32_t idx = (probe < max_idx) ? probe : max_idx - 1;
      hash = mix(hash, values[idx]);
    }
  }
  return hash;
}