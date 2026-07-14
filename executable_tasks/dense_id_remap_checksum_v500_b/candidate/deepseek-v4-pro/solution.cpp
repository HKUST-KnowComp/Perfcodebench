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
  if (iters <= 0) return 0;
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t probe : probes) {
    const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
    hash = mix(hash, values[idx]);
  }
  return hash;
}
