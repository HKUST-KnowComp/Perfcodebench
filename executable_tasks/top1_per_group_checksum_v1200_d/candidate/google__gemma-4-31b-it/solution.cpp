#include "interface.h"
#include <vector>
#include <algorithm>

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
  
  // Use a vector as a direct-mapped array for O(1) access and better cache locality
  // compared to std::unordered_map.
  std::vector<uint32_t> best(group_count, 0);
  uint64_t final_hash = 0;
  
  const size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the best values for this iteration
    std::fill(best.begin(), best.end(), 0);

    // Find max value per group
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute checksum for this iteration
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      current_hash = mix(current_hash, best[g]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}