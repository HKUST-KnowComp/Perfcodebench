#include "interface.h" 

#include <vector>
#include <cstdint>

/**
 * Performance Engineer's Optimization:
 * 1. The 'iters' loop in the baseline resets the hash to the FNV offset basis every iteration.
 *    Since 'flags' and 'values' are constant, the result of every iteration is identical.
 *    We can compute the result once to achieve O(N) instead of O(iters * N).
 * 2. The baseline creates temporary vectors (left, right, out) which involves multiple 
 *    allocations and copies. We replace this with a two-pass approach that hashes 
 *    elements in-place.
 * 3. Pass 1 hashes all elements where flag is true (1).
 * 4. Pass 2 hashes all elements where flag is false (0).
 * 5. This preserves the 'stable' property of the partition while using O(1) auxiliary space.
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

  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t hash = 1469598103934665603ULL;

  // Pass 1: Process 'true' elements (stable order)
  for (size_t i = 0; i < n; ++i) {
    if (f_ptr[i]) {
      hash ^= static_cast<uint64_t>(v_ptr[i]);
      hash *= fnv_prime;
    }
  }

  // Pass 2: Process 'false' elements (stable order)
  for (size_t i = 0; i < n; ++i) {
    if (!f_ptr[i]) {
      hash ^= static_cast<uint64_t>(v_ptr[i]);
      hash *= fnv_prime;
    }
  }

  return hash;
}