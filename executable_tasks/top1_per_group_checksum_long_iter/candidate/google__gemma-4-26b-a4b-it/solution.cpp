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
  // Use a vector instead of unordered_map for O(1) direct indexing.
  // Since groups are dense and range from 0 to group_count-1, this is optimal.
  std::vector<uint32_t> best(group_count);
  size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the best values for this iteration
    std::fill(best.begin(), best.end(), 0);

    // Single pass to find max per group
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
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

  // The original code re-initializes hash inside the loop but returns the last one.
  // However, the logic implies the hash is computed per iteration and the final result is returned.
  // To match the baseline's behavior exactly, we must ensure the hash is calculated correctly.
  // The baseline code: hash = 1469598103934665603ULL; inside the loop, then it updates hash.
  // The loop structure in the baseline actually overwrites 'hash' every iteration.
  
  // Re-calculating the final hash to match the baseline's exact logic flow:
  uint64_t final_hash = 1469598103934665603ULL;
  // We need to re-run the last iteration's logic to ensure correctness if iters > 0
  // But wait, the baseline's 'hash' variable is updated in every iteration.
  // Let's re-simulate the baseline's loop exactly.
  
  // Correcting the loop to match baseline behavior:
  uint64_t current_hash = 0;
  std::fill(best.begin(), best.end(), 0);
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(best.begin(), best.end(), 0);
    for (size_t i = 0; i < n; ++i) {
      if (v_ptr[i] > best[g_ptr[i]]) {
        best[g_ptr[i]] = v_ptr[i];
      }
    }
    current_hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      current_hash = mix(current_hash, static_cast<uint64_t>(best[g]));
    }
  }
  
  return current_hash;
}