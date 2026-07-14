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

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Preallocate dense group maximum array once, fits in L1 cache for small groups
  std::vector<uint32_t> best(group_count, 0u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset group maxima for this iteration (extremely fast for small group_count)
    std::fill(best.begin(), best.end(), 0u);

    // Process all elements with direct, cache-friendly indexing
    for (std::size_t i = 0; i < groups.size(); ++i) {
      const uint32_t g = groups[i];
      const uint32_t v = values[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute checksum exactly matching original implementation
    final_hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      final_hash = mix(final_hash, best[g]);
    }
  }
  return final_hash;
}