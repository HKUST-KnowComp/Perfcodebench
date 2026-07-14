#include "interface.h"

#include <algorithm>
#include <cstdint>
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
  uint64_t hash = 1469598103934665603ULL;
  const auto keys_begin = keys.begin();
  const auto keys_end = keys.begin() + keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = hash;
    for (uint32_t probe : probes) {
      const auto it = std::lower_bound(keys_begin, keys_end, probe);
      if (it != keys_end && *it == probe) {
        const std::size_t idx = static_cast<std::size_t>(it - keys_begin);
        current_hash = mix(current_hash, values[idx]);
      }
    }
    hash = current_hash;
  }
  return hash;
}