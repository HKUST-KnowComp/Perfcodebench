#include "interface.h"
#include <vector>
#include <cstring>
#include <algorithm>

namespace {

/**
 * FNV-1a mix function as specified in the baseline.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  
  // Pre-allocate a buffer to store the maximum value for each group.
  // Since group IDs are dense and up to group_count, a vector/array is much faster than a map.
  std::vector<uint32_t> max_values(group_count);
  uint64_t final_hash = 0;
  
  const size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the max_values buffer for each iteration.
    std::memset(max_values.data(), 0, group_count * sizeof(uint32_t));

    // Compute the maximum value for each group.
    // This loop is the primary bottleneck; we use raw pointers for speed.
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > max_values[g]) {
        max_values[g] = v;
      }
    }

    // Compute the checksum hash across all group maxima.
    uint64_t current_hash = FNV_OFFSET_BASIS;
    const uint32_t* m_ptr = max_values.data();
    for (uint32_t g = 0; g < group_count; ++g) {
      current_hash = mix(current_hash, m_ptr[g]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}
