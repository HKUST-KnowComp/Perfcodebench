#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Performance Engineer's Optimization Notes:
 * 1. Redundancy Elimination: The original code re-calculates the same hash 'iters' times 
 *    because 'flags' and 'values' are constant and the hash basis is reset inside the loop. 
 *    We compute it once to achieve O(N) instead of O(N * iters).
 * 2. Memory Allocation: Removed 'left', 'right', and 'out' vectors. These caused 
 *    significant overhead due to dynamic allocation and multiple data copies.
 * 3. Two-Pass Hashing: By iterating over the data twice (first for true flags, then for 
 *    false flags), we simulate the stable partition order directly into the hash 
 *    function, avoiding the need for intermediate storage.
 * 4. FNV-1a Constants: Used the 64-bit FNV-1a prime and offset basis as specified in 
 *    the baseline logic.
 */

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  
  if (iters <= 0) {
    return 0;
  }

  const size_t n = values.size();
  if (n == 0) {
    return 1469598103934665603ULL;
  }

  const uint8_t* __restrict f_ptr = flags.data();
  const uint32_t* __restrict v_ptr = values.data();

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  // Pass 1: Process elements where flag is true (non-zero)
  // This corresponds to the 'left' partition.
  for (size_t i = 0; i < n; ++i) {
    if (f_ptr[i]) {
      hash ^= static_cast<uint64_t>(v_ptr[i]);
      hash *= prime;
    }
  }

  // Pass 2: Process elements where flag is false (zero)
  // This corresponds to the 'right' partition.
  for (size_t i = 0; i < n; ++i) {
    if (!f_ptr[i]) {
      hash ^= static_cast<uint64_t>(v_ptr[i]);
      hash *= prime;
    }
  }

  return hash;
}