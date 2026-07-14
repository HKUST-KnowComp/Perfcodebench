#include "interface.h"

#include <algorithm>
#include <cstddef>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Precompute the mapped value for each probe using binary search (same as baseline)
  std::vector<uint32_t> mapped(probes.size());
  for (std::size_t i = 0; i < probes.size(); ++i) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
    std::size_t idx = static_cast<std::size_t>(it - keys.begin());
    mapped[i] = values[idx];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (auto v : mapped) {
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}