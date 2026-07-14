#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> counts(rows, 0);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts efficiently
    std::fill(counts.begin(), counts.end(), 0);
    
    // Count occurrences of each row (single pass)
    for (uint32_t row : row_ids) {
      ++counts[row];
    }
    
    // Compute offsets (prefix sums) and hash
    hash = 1469598103934665603ULL;
    uint32_t offset = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      hash = mix(hash, offset);
      offset += counts[r];
    }
    hash = mix(hash, offset);  // Final offset
  }
  
  return hash;
}