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
  if (iters <= 0) return 0;

  const uint32_t* k_data = keys.data();
  const uint32_t* v_data = values.data();
  const size_t k_size = keys.size();

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t probe : probes) {
    auto it = std::lower_bound(k_data, k_data + k_size, probe);
    hash = mix(hash, v_data[static_cast<size_t>(it - k_data)]);
  }
  return hash;
}