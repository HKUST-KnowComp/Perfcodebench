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
  
  std::vector<uint64_t> accum(domain, 0);
  std::vector<uint32_t> touched;
  touched.reserve(indices.size());
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Sparse clear: only reset indices that were touched
    for (uint32_t idx : touched) {
      accum[idx] = 0;
    }
    touched.clear();
    
    // Accumulate values
    for (std::size_t i = 0; i < indices.size(); ++i) {
      uint32_t idx = indices[i];
      if (accum[idx] == 0) {
        touched.push_back(idx);
      }
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
    
    // Compute hash over entire domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
  }
  
  return hash;
}
