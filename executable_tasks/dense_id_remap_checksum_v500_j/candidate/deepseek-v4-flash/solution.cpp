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
  const size_t n = probes.size();
  // Precompute remapped values once
  std::vector<uint32_t> remapped(n);
  for (size_t i = 0; i < n; ++i) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
    size_t idx = static_cast<size_t>(it - keys.begin());
    remapped[i] = values[idx];
  }

  uint64_t hash = 0;
  const uint64_t init = 1469598103934665603ULL;
  const uint32_t* remapped_ptr = remapped.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = init;
    for (size_t i = 0; i < n; ++i) {
      h ^= remapped_ptr[i];
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}