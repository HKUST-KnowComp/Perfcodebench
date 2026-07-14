#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  if (iters <= 0) return 0;
  // Precompute partition layout once for all iterations
  std::vector<uint32_t> offsets(groups + 1, 0);
  std::vector<uint32_t> rows;
  // Count frequency per group (only keys in [0, groups-1] matter)
  std::vector<uint32_t> counts(groups, 0);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    uint32_t k = keys[i];
    if (k < groups) ++counts[k];
  }
  // Build prefix sum of counts to get offsets and total rows size
  for (uint32_t g = 0; g < groups; ++g) {
    offsets[g + 1] = offsets[g] + counts[g];
  }
  rows.resize(offsets[groups]);
  // Scatter row indices into rows array in input order, preserving per-group order
  std::vector<uint32_t> next_pos = offsets;  // start positions for each group
  for (std::size_t i = 0; i < keys.size(); ++i) {
    uint32_t k = keys[i];
    if (k < groups) {
      rows[next_pos[k]++] = static_cast<uint32_t>(i);
    }
  }
  // Compute hash iters times
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
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
