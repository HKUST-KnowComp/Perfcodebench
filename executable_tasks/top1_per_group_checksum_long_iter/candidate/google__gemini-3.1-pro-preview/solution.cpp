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
  
  // Use a vector for dense group storage to avoid unordered_map overhead.
  // Pre-allocating outside the loop to reuse memory.
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  
  const size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();
  uint32_t* best_ptr = best.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the best values for each group.
    std::memset(best_ptr, 0, group_count * sizeof(uint32_t));

    // Aggregate maximums.
    for (size_t i = 0; i < n; ++i) {
      uint32_t group = g_ptr[i];
      uint32_t value = v_ptr[i];
      if (value > best_ptr[group]) {
        best_ptr[group] = value;
      }
    }

    // Compute the FNV-1a-like checksum.
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best_ptr[g]);
    }
  }

  return hash;
}
