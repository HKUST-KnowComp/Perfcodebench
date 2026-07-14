#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    std::fill(offsets.begin(), offsets.end(), 0);

    // Since row_ids are sorted, we can count occurrences efficiently
    // row_ids[i] is the row index of the i-th element.
    // The offset for row r is the number of elements in rows 0 to r-1.
    // Therefore, offsets[r+1] = count of elements with row_id <= r.
    
    size_t nnz = row_ids.size();
    if (nnz > 0) {
      size_t current_row = 0;
      for (size_t i = 0; i < nnz; ++i) {
        uint32_t row = row_ids[i];
        // All rows from current_row to row-1 have 0 elements starting from current_row
        // But we only need to mark the end of the current row.
        // The number of elements in row 'row' is (i+1) - (first index of row).
        // Actually, the simplest way to build CSR offsets from sorted COO is:
        // offsets[row + 1] = i + 1; // This is not quite right because it doesn't handle empty rows.
      }
    }

    // Correct logic for sorted row_ids to CSR offsets:
    // offsets[0] = 0
    // offsets[r+1] = count of elements in row_ids < r+1
    size_t count = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      while (count < nnz && row_ids[count] == r) {
        count++;
      }
      offsets[r + 1] = static_cast<uint32_t>(count);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}
