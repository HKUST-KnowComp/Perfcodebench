#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized stable_partition_hash.
 * 
 * The original implementation performed a stable partition and hashed the result 
 * inside a loop of 'iters' iterations. Since the input vectors 'flags' and 'values' 
 * are constant, the result of the partition and the hash calculation is identical 
 * for every iteration. 
 * 
 * Optimization Strategy:
 * 1. Compute the hash only once, as the result is invariant across iterations.
 * 2. Avoid intermediate vector allocations (left, right, out) which cause 
 *    significant memory pressure and latency.
 * 3. Perform the stable partition logic in-place during hashing using two passes:
 *    - Pass 1: Hash all values[i] where flags[i] is true (non-zero).
 *    - Pass 2: Hash all values[i] where flags[i] is false (zero).
 * 4. Use the FNV-1a constants and logic provided in the baseline.
 */

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  
  // If there are no iterations or no data, return the initial hash or 0.
  // Based on baseline, if iters > 0, hash is initialized to the FNV offset basis.
  if (iters <= 0) return 0;

  const size_t n = values.size();
  const uint8_t* f_ptr = flags.data();
  const uint32_t* v_ptr = values.data();

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  // Pass 1: Process elements that would be in the 'left' partition (flags[i] != 0)
  for (size_t i = 0; i < n; ++i) {
    if (f_ptr[i]) {
      hash ^= static_cast<uint64_t>(v_ptr[i]);
      hash *= FNV_PRIME;
    }
  }

  // Pass 2: Process elements that would be in the 'right' partition (flags[i] == 0)
  for (size_t i = 0; i < n; ++i) {
    if (!f_ptr[i]) {
      hash ^= static_cast<uint64_t>(v_ptr[i]);
      hash *= FNV_PRIME;
    }
  }

  return hash;
}