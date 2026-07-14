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
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (uint32_t i = 0; i <= rows; ++i) offsets[i] = 0;

    // Since row_ids is sorted, we can count occurrences efficiently
    // row_ids contains the row index for each non-zero element
    // The number of elements in row r is the count of r in row_ids
    uint32_t current_row = 0;
    uint32_t count = 0;
    for (uint32_t row : row_ids) {
      while (current_row < row) {
        offsets[current_row + 1] = offsets[current_row];
        current_row++;
      }
      count++;
    }
    
    // Fill remaining rows if row_ids ended before the last row
    while (current_row < rows) {
      offsets[current_row + 1] = offsets[current_row];
      current_row++;
    }
    
    // The logic above is slightly flawed for prefix sums. 
    // Correct approach for sorted COO to CSR offsets:
    // offsets[i] is the number of elements in rows < i.
    
    // Re-implementing correctly:
    uint32_t nz_ptr = 0;
    uint32_t total_nz = static_cast<uint32_t>(row_ids.size());
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r] = nz_ptr;
      while (nz_ptr < total_nz && row_ids[nz_ptr] == r) {
        nz_ptr++;
      }
    }
    offsets[rows] = total_nz;

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}