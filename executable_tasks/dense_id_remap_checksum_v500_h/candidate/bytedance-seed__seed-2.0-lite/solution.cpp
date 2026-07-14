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
  if (keys.empty()) return 0;

  const uint32_t max_key = keys.back();
  std::vector<uint32_t> lut(max_key + 1);
  for (size_t i = 0; i < keys.size(); ++i) {
    lut[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      hash = mix(hash, lut[probe]);
    }
  }
  return hash;
}