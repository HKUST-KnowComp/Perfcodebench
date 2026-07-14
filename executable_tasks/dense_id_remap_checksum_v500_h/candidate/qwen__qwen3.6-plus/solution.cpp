#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  std::vector<uint32_t> remapped;
  remapped.reserve(probes.size());
  for (uint32_t p : probes) {
    auto it = std::lower_bound(keys.begin(), keys.end(), p);
    remapped.push_back(values[static_cast<std::size_t>(it - keys.begin())]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : remapped) {
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}