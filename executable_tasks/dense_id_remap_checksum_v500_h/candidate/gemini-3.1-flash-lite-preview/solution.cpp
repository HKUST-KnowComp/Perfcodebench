#include "interface.h"
#include <vector>
#include <algorithm>

namespace {
inline uint64_t mix(uint64_t hash, uint32_t value) {
  hash ^= static_cast<uint64_t>(value);
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

  uint32_t max_key = 0;
  for (uint32_t k : keys) if (k > max_key) max_key = k;

  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  const uint32_t* lookup_ptr = lookup.data();
  const uint32_t* probes_ptr = probes.data();
  const size_t probes_size = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < probes_size; ++i) {
      current_hash = mix(current_hash, lookup_ptr[probes_ptr[i]]);
    }
    hash = current_hash;
  }
  return hash;
}