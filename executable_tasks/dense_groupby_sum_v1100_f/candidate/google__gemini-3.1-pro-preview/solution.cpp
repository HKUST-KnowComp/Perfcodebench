#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

namespace {

/**
 * Computes the FNV-1a-style checksum for the aggregated sums.
 * This matches the logic in the baseline but operates on a dense array.
 */
inline uint64_t compute_checksum(const uint64_t* agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    const uint64_t v = agg[g];
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  if (groups <= 0) return 1469598103934665603ULL; // Default FNV hash for empty set

  // Use a contiguous vector for the accumulator to ensure O(1) access and cache efficiency.
  // Since keys are bounded [0, groups-1], this is the most efficient structure.
  std::vector<uint64_t> agg(groups);
  uint64_t* agg_ptr = agg.data();
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  const size_t n = keys.size();

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the accumulator for each iteration.
    std::memset(agg_ptr, 0, groups * sizeof(uint64_t));

    // Perform the aggregation. Unrolling helps reduce loop overhead and improves instruction-level parallelism.
    size_t i = 0;
    for (; i + 3 < n; i += 4) {
      agg_ptr[keys_ptr[i]] += values_ptr[i];
      agg_ptr[keys_ptr[i + 1]] += values_ptr[i + 1];
      agg_ptr[keys_ptr[i + 2]] += values_ptr[i + 2];
      agg_ptr[keys_ptr[i + 3]] += values_ptr[i + 3];
    }
    for (; i < n; ++i) {
      agg_ptr[keys_ptr[i]] += values_ptr[i];
    }

    // The task requires returning the checksum of the grouped sums.
    // We compute it inside the loop to ensure 'out' is updated as per the baseline logic,
    // though only the result of the last iteration is returned.
    final_hash = compute_checksum(agg_ptr, groups);
  }

  return final_hash;
}
