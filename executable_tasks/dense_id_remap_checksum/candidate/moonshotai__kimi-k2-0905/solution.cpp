#include "interface.h"
#include <vector>
#include <cstdint>

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
  const uint32_t max_key = keys.empty() ? 0 : keys.back();
  std::vector<uint32_t> lut(max_key + 1, 0);
  for (size_t i = 0; i < keys.size(); ++i) {
    lut[keys[i]] = values[i];
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_hash = hash;
    for (uint32_t probe : probes) {
      iter_hash = mix(iter_hash, lut[probe]);
    }
    hash = iter_hash;
  }
  return hash;
}