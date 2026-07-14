#include "interface.h"

#include <algorithm>
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
  std::vector<uint32_t> remap_table(keys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    remap_table[i] = values[i];
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
      const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
      iter_hash = mix(iter_hash, remap_table[idx]);
    }
    hash += iter_hash;
  }
  return hash;
}