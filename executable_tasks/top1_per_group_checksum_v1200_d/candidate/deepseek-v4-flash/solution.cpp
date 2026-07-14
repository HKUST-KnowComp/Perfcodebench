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

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset the best array to zeros
    // fill is O(group_count), but avoids hash map overhead
    std::fill(best.begin(), best.end(), 0);

    // Accumulate per-group maximum
    for (std::size_t i = 0; i < groups.size(); ++i) {
      uint32_t g = groups[i];
      uint32_t v = values[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute checksum over all groups (0 for groups not updated)
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
    }
  }
  return hash;
}