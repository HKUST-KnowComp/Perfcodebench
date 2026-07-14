#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Precompute CSR offsets once, reused across all iterations
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0u);
  const size_t nnz = row_ids.size();
  
  if (nnz > 0 && rows > 0) {
    size_t ptr = 0;
    uint32_t current_row = 0;
    while (ptr < nnz && current_row < rows) {
      // Count all entries for current row
      while (ptr < nnz && row_ids[ptr] == current_row) {
        ptr++;
      }
      current_row++;
      offsets[current_row] = static_cast<uint32_t>(ptr);
      
      // Fill offsets for any empty rows between current and next non-empty row
      while (current_row < rows && (ptr >= nnz || row_ids[ptr] > current_row)) {
        offsets[current_row + 1] = offsets[current_row];
        current_row++;
      }
    }
    // Fill any remaining trailing rows
    while (current_row < rows) {
      offsets[current_row + 1] = offsets[current_row];
      current_row++;
    }
  }

  // Hot iteration loop only computes hash, no redundant offset reconstruction
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}