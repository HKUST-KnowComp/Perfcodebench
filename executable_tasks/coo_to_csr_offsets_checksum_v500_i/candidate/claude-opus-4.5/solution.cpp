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

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const size_t n = row_ids.size();
  const size_t offset_size = static_cast<size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(offset_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Fast zero using memset
    std::memset(offsets.data(), 0, offset_size * sizeof(uint32_t));
    
    if (n > 0) {
      // Since row_ids is sorted, we can build offsets directly
      // by counting elements per row in a single pass
      const uint32_t* __restrict__ rid = row_ids.data();
      uint32_t* __restrict__ off = offsets.data();
      
      // Count occurrences of each row
      for (size_t i = 0; i < n; ++i) {
        off[rid[i] + 1]++;
      }
      
      // Prefix sum to get offsets
      for (uint32_t r = 1; r <= rows; ++r) {
        off[r] += off[r - 1];
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* __restrict__ off = offsets.data();
    for (size_t i = 0; i < offset_size; ++i) {
      hash = mix(hash, off[i]);
    }
  }
  
  return hash;
}