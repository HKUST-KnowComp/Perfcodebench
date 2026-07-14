#include "interface.h"

#include <vector>
#include <cstring>
#include <algorithm>

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
  
  // Precompute unique indices for sparse clearing
  std::vector<uint32_t> unique_indices;
  unique_indices.reserve(indices.size());
  {
    std::vector<bool> seen(domain, false);
    for (std::size_t i = 0; i < indices.size(); ++i) {
      uint32_t idx = indices[i];
      if (!seen[idx]) {
        seen[idx] = true;
        unique_indices.push_back(idx);
      }
    }
  }
  
  const std::size_t n = indices.size();
  const uint32_t* __restrict idx_ptr = indices.data();
  const uint32_t* __restrict val_ptr = values.data();
  uint64_t* __restrict acc_ptr = accum.data();
  
  // Threshold: if unique indices are sparse enough, clear only those
  // Otherwise use memset for the whole array
  const bool use_sparse_clear = unique_indices.size() < domain / 4;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear accumulator
    if (iter > 0) {
      if (use_sparse_clear) {
        for (uint32_t idx : unique_indices) {
          acc_ptr[idx] = 0;
        }
      } else {
        std::memset(acc_ptr, 0, domain * sizeof(uint64_t));
      }
    }
    
    // Accumulate
    for (std::size_t i = 0; i < n; ++i) {
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