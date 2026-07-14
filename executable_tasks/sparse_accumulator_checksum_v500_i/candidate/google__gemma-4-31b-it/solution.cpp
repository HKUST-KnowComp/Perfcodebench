#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain, 
    int iters) {
  
  // Use a dense vector for the accumulator to avoid map overhead.
  // Pre-allocate to avoid repeated allocations across iterations.
  std::vector<uint64_t> accum(domain, 0);
  
  const uint32_t* idx_ptr = indices.data();
  const uint32_t* val_ptr = values.data();
  const size_t n = indices.size();
  
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset accumulator efficiently
    std::fill(accum.begin(), accum.end(), 0);

    // Accumulate weighted updates
    for (size_t i = 0; i < n; ++i) {
      accum[idx_ptr[i]] += static_cast<uint64_t>(val_ptr[i]);
    }

    // Compute checksum for the current iteration
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      current_hash = mix(current_hash, accum[key]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}