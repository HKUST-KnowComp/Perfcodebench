#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  if (keys.empty()) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g <= groups; ++g) {
      hash = mix(hash, 0);
    }
    return hash;
  }

  const size_t n = keys.size();
  std::vector<uint32_t> counts(groups, 0);
  std::vector<uint32_t> offsets(groups + 1, 0);
  std::vector<uint32_t> rows(n);
  
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    for (uint32_t g = 0; g < groups; ++g) {
      counts[g] = 0;
    }

    // Pass 1: Count frequencies
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      if (k < groups) {
        counts[k]++;
      }
    }

    // Pass 2: Compute offsets (prefix sums)
    offsets[0] = 0;
    for (uint32_t g = 0; g < groups; ++g) {
      offsets[g + 1] = offsets[g] + counts[g];
    }

    // Pass 3: Scatter rows using a temporary cursor array to avoid modifying offsets
    // We use a local copy of offsets to track current insertion position per group
    std::vector<uint32_t> cursors = offsets;
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      if (k < groups) {
        rows[cursors[k]++] = static_cast<uint32_t>(i);
      }
    }

    // Compute checksum
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
