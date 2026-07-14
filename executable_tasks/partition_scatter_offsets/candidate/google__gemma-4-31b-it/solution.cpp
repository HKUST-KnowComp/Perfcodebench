#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  const size_t n = keys.size();
  std::vector<uint32_t> offsets(groups + 1);
  std::vector<uint32_t> rows(n);
  std::vector<uint32_t> current_pos(groups);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    for (uint32_t g = 0; g < groups; ++g) {
      offsets[g] = 0;
    }

    // Pass 1: Count frequencies
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = keys[i];
      if (key < groups) {
        offsets[key]++;
      }
    }

    // Compute prefix sums for offsets
    uint32_t cumulative = 0;
    for (uint32_t g = 0; g < groups; ++g) {
      uint32_t count = offsets[g];
      offsets[g] = cumulative;
      current_pos[g] = cumulative;
      cumulative += count;
    }
    offsets[groups] = cumulative;

    // Pass 2: Scatter rows
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = keys[i];
      if (key < groups) {
        rows[current_pos[key]++] = static_cast<uint32_t>(i);
      }
    }

    // Calculate checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    // Only hash the rows that were actually placed (up to cumulative)
    for (size_t i = 0; i < (size_t)cumulative; ++i) {
      hash = mix(hash, rows[i]);
    }
  }
  return hash;
}