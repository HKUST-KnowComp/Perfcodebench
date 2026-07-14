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
  
  std::vector<uint64_t> accum(domain, 0ULL);
  uint64_t hash = 0;
  
  const size_t n = indices.size();
  const uint32_t* __restrict idx_ptr = indices.data();
  const uint32_t* __restrict val_ptr = values.data();
  uint64_t* __restrict acc_ptr = accum.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear only the indices that were touched
    for (size_t i = 0; i < n; ++i) {
      acc_ptr[idx_ptr[i]] = 0;
    }
    
    // Accumulate
    for (size_t i = 0; i < n; ++i) {
      acc_ptr[idx_ptr[i]] += static_cast<uint64_t>(val_ptr[i]);
    }
    
    // Compute hash over entire domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, acc_ptr[key]);
    }
  }
  
  return hash;
}