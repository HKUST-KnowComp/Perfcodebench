#include "interface.h"

#include <algorithm>
#include <cstddef>

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

  const std::size_t num_probes = probes.size();
  std::vector<uint32_t> remapped(num_probes);

  // Check if keys form a contiguous range (dense layout)
  const bool contiguous = (!keys.empty()) &&
      (static_cast<std::size_t>(keys.back()) - static_cast<std::size_t>(keys.front()) + 1 == keys.size());

  if (contiguous) {
    const uint32_t base = keys.front();
    for (std::size_t i = 0; i < num_probes; ++i) {
      // Assumes every probe is present in keys; direct indexing
      remapped[i] = values[probes[i] - base];
    }
  } else {
    for (std::size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = probes[i];
      // Binary search (keys is sorted)
      auto it = std::lower_bound(keys.begin(), keys.end(), probe);
      // Assume probe is always found; it - begin() gives index
      const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
      remapped[i] = values[idx];
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < num_probes; ++i) {
      hash = mix(hash, remapped[i]);
    }
  }

  return hash;
}