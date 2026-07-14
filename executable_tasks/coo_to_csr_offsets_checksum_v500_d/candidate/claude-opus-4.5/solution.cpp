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
  const size_t offset_size = static_cast<size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(offset_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Zero out offsets
    for (size_t i = 0; i < offset_size; ++i) {
      offsets[i] = 0;
    }
    
    // Count occurrences of each row (store in offsets[row+1])
    const uint32_t* __restrict__ row_ptr = row_ids.data();
    for (size_t i = 0; i < n; ++i) {
      ++offsets[row_ptr[i] + 1];
    }
    
    // Prefix sum to get CSR offsets
    uint32_t cumsum = 0;
    for (size_t i = 0; i < offset_size; ++i) {
      cumsum += offsets[i];
      offsets[i] = cumsum;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* __restrict__ off_ptr = offsets.data();
    for (size_t i = 0; i < offset_size; ++i) {
      hash = mix(hash, off_ptr[i]);
    }
  }
  
  return hash;
}
