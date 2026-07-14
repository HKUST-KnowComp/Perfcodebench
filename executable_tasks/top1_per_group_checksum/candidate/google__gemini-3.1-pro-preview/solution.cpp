#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>
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
  
  // Pre-allocate buffer to store maximums for each group.
  // Using a vector for automatic memory management while ensuring contiguous storage.
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;
  
  const size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the buffer for the current iteration.
    // std::fill or memset is significantly faster than map.clear().
    std::fill(best.begin(), best.end(), 0u);

    // Process all elements to find the maximum per group.
    // Using a raw pointer and std::max to help the compiler generate cmov instructions.
    for (size_t i = 0; i < n; ++i) {
      uint32_t group_idx = g_ptr[i];
      uint32_t val = v_ptr[i];
      if (val > best[group_idx]) {
          best[group_idx] = val;
      }
    }

    // Compute the checksum based on the final maximums.
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, static_cast<uint64_t>(best[g]));
    }
  }

  return hash;
}
