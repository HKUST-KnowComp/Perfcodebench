#include "interface.h"

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
  if (keys.empty()) return 0;
  const uint32_t base_key = keys[0];
  uint64_t final_hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      const size_t idx = static_cast<size_t>(probe - base_key);
      hash = mix(hash, values[idx]);
    }
    final_hash = hash;
  }
  return final_hash;
}