#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  // Preserve baseline behavior: if iters == 0, return 0 without doing any work.
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = keys.size();

  // Count occurrences per group (ignore keys >= groups)
  std::vector<uint32_t> counts;
  counts.assign(static_cast<std::size_t>(groups), 0u);
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t k = keys[i];
    if (k < groups) {
      counts[static_cast<std::size_t>(k)] += 1u;
    }
  }

  // Compute prefix offsets
  std::vector<uint32_t> offsets;
  offsets.resize(static_cast<std::size_t>(groups) + 1u);
  offsets[0] = 0u;
  for (uint32_t g = 0; g < groups; ++g) {
    offsets[static_cast<std::size_t>(g + 1u)] = offsets[static_cast<std::size_t>(g)] + counts[static_cast<std::size_t>(g)];
  }

  // Scatter rows into partitions preserving order
  const uint32_t total = offsets[static_cast<std::size_t>(groups)];
  std::vector<uint32_t> rows;
  rows.resize(total);
  if (groups != 0u) {
    std::vector<uint32_t> pos;
    pos.resize(static_cast<std::size_t>(groups));
    for (uint32_t g = 0; g < groups; ++g) {
      pos[static_cast<std::size_t>(g)] = offsets[static_cast<std::size_t>(g)];
    }
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      if (k < groups) {
        uint32_t &p = pos[static_cast<std::size_t>(k)];
        rows[static_cast<std::size_t>(p)] = static_cast<uint32_t>(i);
        ++p;
      }
    }
  }

  // Compute checksum as in baseline (FNV-1a-like mixing over offsets then rows)
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < offsets.size(); ++i) {
    hash = mix(hash, offsets[i]);
  }
  for (std::size_t i = 0; i < rows.size(); ++i) {
    hash = mix(hash, rows[i]);
  }

  return hash;
}
