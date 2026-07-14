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
  std::vector<uint32_t> offsets;
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts efficiently
    std::fill(counts.begin(), counts.end(), 0);
    
    // Count row occurrences in single pass
    for (uint32_t row : row_ids) {
      ++counts[row];
    }
    
    // Build offsets in single pass
    offsets.resize(rows + 1);
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] = offsets[r] + counts[r];
    }
    
    // Compute hash (deterministic, same result each iteration)
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  
  return hash;
}