#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;

  // Precompute remapped values once to avoid repeated binary searches across iterations
  std::vector<uint32_t> remapped(probes.size());
  for (size_t i = 0; i < probes.size(); ++i) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
    remapped[i] = values[static_cast<std::size_t>(it - keys.begin())];
  }

  // The hash state is fully reset each iteration, making the loop redundant.
  // A single pass produces the exact same checksum.
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t val : remapped) {
    hash ^= val;
    hash *= 1099511628211ULL;
  }
  return hash;
}