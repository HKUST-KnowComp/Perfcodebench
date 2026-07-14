#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Use a vector as a direct-address table for O(1) access and cache efficiency.
  // This is significantly faster than std::unordered_map for dense group IDs.
  std::vector<uint32_t> best(group_count);
  const size_t n = groups.size();
  const uint32_t* groups_ptr = groups.data();
  const uint32_t* values_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the best values for this iteration
    std::fill(best.begin(), best.end(), 0);

    // Single pass to find the maximum value per group
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = groups_ptr[i];
      uint32_t v = values_ptr[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute the FNV-1a style hash
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, static_cast<uint64_t>(best[g]));
    }
  }

  // The problem implies the hash is returned from the last iteration's calculation.
  // However, the baseline logic re-initializes hash inside the loop. 
  // To match the baseline's behavior exactly, we must ensure the final hash returned 
  // is the one calculated in the last iteration.
  
  // Re-calculating the final hash to ensure correctness with the baseline's structure
  std::fill(best.begin(), best.end(), 0);
  for (size_t i = 0; i < n; ++i) {
    uint32_t g = groups_ptr[i];
    uint32_t v = values_ptr[i];
    if (v > best[g]) best[g] = v;
  }
  
  uint64_t final_hash = 1469598103934665603ULL;
  for (uint32_t g = 0; g < group_count; ++g) {
    final_hash = mix(final_hash, static_cast<uint64_t>(best[g]));
  }

  // Wait, the baseline loop structure is: 
  // for iter: { best.clear(); for i: { ... }; hash = FNV_INIT; for g: { hash = mix(hash, best[g]); } }
  // This means the 'hash' variable is overwritten every iteration. 
  // The final value returned is the hash from the LAST iteration.
  
  // Let's refine the loop to be exactly equivalent but faster.
  uint64_t current_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(best.begin(), best.end(), 0);
    for (size_t i = 0; i < n; ++i) {
      if (values_ptr[i] > best[groups_ptr[i]]) {
        best[groups_ptr[i]] = values_ptr[i];
      }
    }
    current_hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      current_hash = mix(current_hash, static_cast<uint64_t>(best[g]));
    }
  }
  
  return current_hash;
}