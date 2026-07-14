#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t kInitialHash = 1469598103934665603ULL;
constexpr uint64_t kMixMultiplier = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= kMixMultiplier;
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  if (iters <= 0) {
    return 0;
  }
  // Precompute per-group maximums once.
  std::vector<uint32_t> maxes(group_count, 0);
  const std::size_t n = groups.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t g = groups[i];
    const uint32_t v = values[i];
    if (v > maxes[g]) {
      maxes[g] = v;
    }
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kInitialHash;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, maxes[g]);
    }
  }
  return hash;
}