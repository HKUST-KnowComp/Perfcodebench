#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= static_cast<uint64_t>(value);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  
  const size_t n = values.size();
  if (n == 0) return 0;

  // Pre-allocate a buffer to store the partitioned result to avoid reallocations.
  std::vector<uint32_t> out(n);
  uint64_t final_hash = 0;

  const uint8_t* f_ptr = flags.data();
  const uint32_t* v_ptr = values.data();
  uint32_t* out_ptr = out.data();

  for (int iter = 0; iter < iters; ++iter) {
    size_t left_idx = 0;
    // First pass: collect elements where flag is true
    for (size_t i = 0; i < n; ++i) {
      if (f_ptr[i]) {
        out_ptr[left_idx++] = v_ptr[i];
      }
    }
    
    size_t right_idx = left_idx;
    // Second pass: collect elements where flag is false
    for (size_t i = 0; i < n; ++i) {
      if (!f_ptr[i]) {
        out_ptr[right_idx++] = v_ptr[i];
      }
    }

    // Compute hash on the partitioned output
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      current_hash = mix(current_hash, out_ptr[i]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}