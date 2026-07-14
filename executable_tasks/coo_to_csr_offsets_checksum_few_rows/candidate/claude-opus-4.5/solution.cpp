#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const size_t n = row_ids.size();
  const uint32_t* __restrict__ data = row_ids.data();
  
  // Pre-allocate counts array - reused across iterations
  std::vector<uint32_t> counts(static_cast<size_t>(rows), 0);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear counts
    for (size_t i = 0; i < rows; ++i) {
      counts[i] = 0;
    }
    
    // Count occurrences - since row_ids is sorted, we can still just iterate
    // For few rows, direct array access is much faster than hash map
    for (size_t i = 0; i < n; ++i) {
      ++counts[data[i]];
    }
    
    // Compute hash with prefix sum inline
    // offsets[0] = 0, offsets[r+1] = offsets[r] + counts[r]
    hash = 1469598103934665603ULL;
    hash = mix(hash, 0);  // offsets[0] = 0
    
    uint32_t prefix = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      prefix += counts[r];
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}
