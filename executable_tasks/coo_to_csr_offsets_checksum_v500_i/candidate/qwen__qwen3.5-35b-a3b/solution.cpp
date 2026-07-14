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
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    offsets.assign(static_cast<std::size_t>(rows) + 1u, 0);
    
    // Count consecutive occurrences of each row (row_ids are sorted)
    uint32_t current_row = 0;
    uint32_t count = 0;
    
    for (uint32_t row : row_ids) {
      if (row == current_row) {
        ++count;
      } else {
        offsets[static_cast<std::size_t>(current_row) + 1u] = count;
        current_row = row;
        count = 1;
      }
    }
    // Handle the last row
    if (!row_ids.empty()) {
      offsets[static_cast<std::size_t>(current_row) + 1u] = count;
    }
    
    // Compute cumulative sum for CSR offsets
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[static_cast<std::size_t>(r + 1u)] += offsets[static_cast<std::size_t>(r)];
    }
    
    // Compute hash of offsets
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  
  return hash;
}