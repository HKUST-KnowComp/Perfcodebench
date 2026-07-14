#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (std::size_t i = 0; i <= rows; ++i) {
      offsets[i] = 0;
    }
    
    // Build offsets from sorted row_ids in single pass
    if (!row_ids.empty()) {
      uint32_t current_row = row_ids[0];
      uint32_t count = 0;
      
      for (uint32_t row : row_ids) {
        if (row != current_row) {
          offsets[static_cast<std::size_t>(current_row) + 1u] = count;
          current_row = row;
          count = 1;
        } else {
          ++count;
        }
      }
      offsets[static_cast<std::size_t>(current_row) + 1u] = count;
      
      // Convert counts to cumulative offsets
      for (std::size_t r = 1; r <= rows; ++r) {
        offsets[r] += offsets[r - 1];
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  
  return hash;
}