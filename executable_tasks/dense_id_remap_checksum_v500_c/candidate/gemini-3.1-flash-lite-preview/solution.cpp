#include "interface.h"
#include <vector>
#include <algorithm>

namespace {
inline uint64_t mix(uint64_t hash, uint32_t value) {
  hash ^= static_cast<uint64_t>(value);
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t remap_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, const std::vector<uint32_t>& probes, int iters) {
  if (keys.empty()) return 0;

  // Pre-calculate the range to determine if a direct lookup table is feasible
  uint32_t min_key = keys.front();
  uint32_t max_key = keys.back();
  uint64_t range = static_cast<uint64_t>(max_key) - min_key + 1;

  // If the range is reasonable, use a direct lookup table for O(1) access
  if (range <= 10000000) {
    std::vector<uint32_t> lookup(range, 0xFFFFFFFF);
    for (size_t i = 0; i < keys.size(); ++i) {
      lookup[keys[i] - min_key] = values[i];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      for (uint32_t probe : probes) {
        uint32_t val = lookup[probe - min_key];
        hash = mix(hash, val);
      }
    }
    return hash;
  } else {
    // Fallback to binary search if range is too large for memory
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      for (uint32_t probe : probes) {
        auto it = std::lower_bound(keys.begin(), keys.end(), probe);
        hash = mix(hash, values[std::distance(keys.begin(), it)]);
      }
    }
    return hash;
  }
}