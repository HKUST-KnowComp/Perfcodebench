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
  std::vector<uint32_t> offsets;
  offsets.resize(rows + 1);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets to 0
    offsets[0] = 0;
    
    // Count consecutive identical row_ids (sorted input)
    uint32_t current_row = 0;
    uint32_t count = 0;
    
    for (uint32_t row : row_ids) {
      if (row == current_row) {
        ++count;
      } else {
        // Fill gaps between rows with same offset
        for (uint32_t r = current_row + 1; r < row; ++r) {
          offsets[r + 1] = offsets[r];
        }
        offsets[row + 1] = offsets[row] + count;
        current_row = row;
        count = 1;
      }
    }
    
    // Fill remaining rows after last row_id
    for (uint32_t r = current_row + 1; r <= rows; ++r) {
      offsets[r] = offsets[r - 1];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  
  return hash;
}