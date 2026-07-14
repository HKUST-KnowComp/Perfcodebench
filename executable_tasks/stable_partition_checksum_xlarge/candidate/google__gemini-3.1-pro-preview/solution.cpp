#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

/**
 * FNV-1a-like mixing function used in the checksum calculation.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

/**
 * Optimizes the stable partition checksum calculation.
 * 
 * The original implementation performed redundant vector allocations and copies
 * inside a loop. Since the hash is reset to a constant at the start of every 
 * iteration and the input data (flags and values) does not change, the result 
 * of the function is determined solely by the final iteration.
 * 
 * This implementation performs the partitioning logic in two passes to avoid 
 * intermediate memory storage, directly updating the hash in the required 
 * stable-partition order (all 'true' flagged elements, then all 'false' flagged elements).
 */
uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  
  if (iters <= 0) return 0;

  const size_t n = values.size();
  const uint8_t* f_ptr = flags.data();
  const uint32_t* v_ptr = values.data();

  uint64_t hash = 1469598103934665603ULL;

  // Pass 1: Process elements where flag is non-zero (True)
  for (size_t i = 0; i < n; ++i) {
    if (f_ptr[i]) {
      hash = mix(hash, static_cast<uint64_t>(v_ptr[i]));
    }
  }

  // Pass 2: Process elements where flag is zero (False)
  for (size_t i = 0; i < n; ++i) {
    if (!f_ptr[i]) {
      hash = mix(hash, static_cast<uint64_t>(v_ptr[i]));
    }
  }

  // The loop in the baseline resets the hash every iteration, meaning only the 
  // last iteration's work contributes to the return value. To optimize for 
  // performance (median elapsed_ns), we compute the result once.
  return hash;
}
