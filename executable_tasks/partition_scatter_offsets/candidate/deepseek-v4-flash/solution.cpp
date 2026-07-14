#include "interface.h"

#include <cstddef>
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
  const std::size_t n = keys.size();
  const std::size_t g = static_cast<std::size_t>(groups);

  // Persistent buffers reused across iterations
  static std::vector<uint32_t> counts;
  static std::vector<uint32_t> offsets;
  static std::vector<uint32_t> rows;
  static std::vector<uint32_t> positions;

  // Ensure sufficient capacity
  if (counts.size() < g) {
    counts.resize(g);
  }
  if (offsets.size() < g + 1) {
    offsets.resize(g + 1);
  }
  if (rows.size() < n) {
    rows.resize(n);
  }
  if (positions.size() < g) {
    positions.resize(g);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // ---- Pass 1: compute histogram ----
    for (std::size_t i = 0; i < n; ++i) {
      ++counts[keys[i]];
    }

    // ---- Compute prefix sums (offsets) ----
    uint32_t sum = 0;
    for (std::size_t i = 0; i < g; ++i) {
      offsets[i] = sum;
      sum += counts[i];
    }
    offsets[g] = sum;

    // ---- Prepare per-group write positions ----
    for (std::size_t i = 0; i < g; ++i) {
      positions[i] = offsets[i];
    }

    // ---- Pass 2: scatter row ids ----
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t k = keys[i];
      rows[positions[k]++] = static_cast<uint32_t>(i);
    }

    // ---- Compute checksum ----
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i <= g; ++i) {
      hash = mix(hash, offsets[i]);
    }
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, rows[i]);
    }

    // ---- Reset counts for next iteration ----
    for (std::size_t i = 0; i < g; ++i) {
      counts[i] = 0;
    }
  }

  return hash;
}
