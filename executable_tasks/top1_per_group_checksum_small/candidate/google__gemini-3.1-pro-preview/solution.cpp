#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  // Since groups are dense and group_count is small, a vector/array is much faster than a map.
  // Pre-allocate to avoid repeated allocations.
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  
  const size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the maximums for each group.
    std::memset(best.data(), 0, group_count * sizeof(uint32_t));

    // Single pass to find the maximum value for each group.
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute the checksum based on the group maximums.
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, static_cast<uint64_t>(best[g]));
    }
  }

  return hash;
}
