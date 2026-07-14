#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

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
  // Build direct-index lookup table once per call.
  // 4 GB covers the entire 32-bit key space.
  static thread_local std::vector<uint32_t> remap(1ULL << 32, 0);

  // Fast fill: memset to zero, then scatter the given mappings.
  std::memset(remap.data(), 0, remap.size() * sizeof(uint32_t));
  const size_t n = keys.size();
  for (size_t i = 0; i < n; ++i) {
    remap[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  const size_t probe_count = probes.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t j = 0; j < probe_count; ++j) {
      hash = mix(hash, remap[probes[j]]);
    }
  }
  return hash;
}