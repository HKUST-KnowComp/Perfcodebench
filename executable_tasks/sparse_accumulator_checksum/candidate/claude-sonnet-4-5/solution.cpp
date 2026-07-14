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
  std::vector<uint32_t> dirty;
  dirty.reserve(indices.size());
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear only dirty indices from previous iteration
    for (uint32_t idx : dirty) {
      accum[idx] = 0;
    }
    dirty.clear();
    
    // Track which indices we touch
    std::vector<bool> touched(domain, false);
    
    // Accumulate with loop unrolling
    std::size_t i = 0;
    std::size_t size = indices.size();
    std::size_t size_unroll = size - (size % 4);
    
    for (; i < size_unroll; i += 4) {
      uint32_t idx0 = indices[i];
      uint32_t idx1 = indices[i + 1];
      uint32_t idx2 = indices[i + 2];
      uint32_t idx3 = indices[i + 3];
      
      accum[idx0] += static_cast<uint64_t>(values[i]);
      accum[idx1] += static_cast<uint64_t>(values[i + 1]);
      accum[idx2] += static_cast<uint64_t>(values[i + 2]);
      accum[idx3] += static_cast<uint64_t>(values[i + 3]);
      
      if (!touched[idx0]) { touched[idx0] = true; dirty.push_back(idx0); }
      if (!touched[idx1]) { touched[idx1] = true; dirty.push_back(idx1); }
      if (!touched[idx2]) { touched[idx2] = true; dirty.push_back(idx2); }
      if (!touched[idx3]) { touched[idx3] = true; dirty.push_back(idx3); }
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      uint32_t idx = indices[i];
      accum[idx] += static_cast<uint64_t>(values[i]);
      if (!touched[idx]) {
        touched[idx] = true;
        dirty.push_back(idx);
      }
    }
    
    // Compute hash over full domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
  }
  
  return hash;
}