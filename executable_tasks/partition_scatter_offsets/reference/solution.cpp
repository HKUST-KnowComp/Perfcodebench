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

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  std::vector<uint32_t> counts(static_cast<std::size_t>(groups), 0);
  std::vector<uint32_t> offsets(static_cast<std::size_t>(groups) + 1, 0);
  std::vector<uint32_t> next(static_cast<std::size_t>(groups), 0);
  std::vector<uint32_t> rows(keys.size(), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);
    for (uint32_t key : keys) {
      ++counts[static_cast<std::size_t>(key)];
    }
    offsets[0] = 0;
    for (uint32_t g = 0; g < groups; ++g) {
      offsets[static_cast<std::size_t>(g + 1)] =
          offsets[static_cast<std::size_t>(g)] + counts[static_cast<std::size_t>(g)];
      next[static_cast<std::size_t>(g)] = offsets[static_cast<std::size_t>(g)];
    }
    for (std::size_t i = 0; i < keys.size(); ++i) {
      const uint32_t key = keys[i];
      rows[static_cast<std::size_t>(next[static_cast<std::size_t>(key)]++)] = static_cast<uint32_t>(i);
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    for (uint32_t v : rows) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
