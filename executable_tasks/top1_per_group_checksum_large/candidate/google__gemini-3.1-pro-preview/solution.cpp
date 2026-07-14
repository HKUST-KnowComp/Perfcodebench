#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstring>

/**
 * Performance Engineer Optimization:
 * 1. Replaced std::unordered_map with a contiguous std::vector<uint32_t>.
 *    Since group_count is provided and groups are dense, an array-based lookup is O(1) and cache-friendly.
 * 2. Moved the allocation of the 'best' array outside the iteration loop to reuse memory.
 * 3. Used std::memset for rapid zero-initialization of the 'best' array.
 * 4. Used raw pointers for 'groups' and 'values' to assist the compiler in loop optimization.
 * 5. Maintained the exact FNV-1a hash logic required for the checksum.
 */

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  
  if (iters <= 0) return 0;

  // Pre-allocate buffer for group maximums
  std::vector<uint32_t> best(group_count);
  uint32_t* __restrict best_ptr = best.data();
  
  const uint32_t* __restrict groups_ptr = groups.data();
  const uint32_t* __restrict values_ptr = values.data();
  const std::size_t n = groups.size();

  uint64_t final_hash = 0;
  const uint64_t fnv_prime = 1099511628211ULL;
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the maximums for this iteration
    std::memset(best_ptr, 0, group_count * sizeof(uint32_t));

    // Compute maximum per group
    // This is the primary bottleneck; sequential access to groups/values 
    // but random access to best_ptr.
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = groups_ptr[i];
      uint32_t v = values_ptr[i];
      if (v > best_ptr[g]) {
        best_ptr[g] = v;
      }
    }

    // Compute FNV-1a hash of the results
    uint64_t current_hash = fnv_offset_basis;
    for (uint32_t g = 0; g < group_count; ++g) {
      current_hash ^= static_cast<uint64_t>(best_ptr[g]);
      current_hash *= fnv_prime;
    }
    final_hash = current_hash;
  }

  return final_hash;
}
