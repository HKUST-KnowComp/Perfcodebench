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
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (size_t i = 0; i < offsets.size(); ++i) offsets[i] = 0;

    // Since row_ids are sorted, we can count occurrences linearly
    // row_ids[i] is the row index of the i-th non-zero element
    // The offset for row r is the number of elements in rows 0 to r-1
    // Therefore, offsets[r+1] = count of elements in rows 0 to r
    
    size_t nnz = row_ids.size();
    for (size_t i = 0; i < nnz; ++i) {
      // The element at index i belongs to row row_ids[i].
      // This means all rows > row_ids[i] must have an offset of at least i+1.
      // However, the standard CSR offset definition is: 
      // offsets[r] = number of elements in rows < r.
      // So we just need to mark the end of each row.
    }
    
    // Correct linear construction for sorted row_ids:
    uint32_t current_row = 0;
    for (size_t i = 0; i < nnz; ++i) {
      uint32_t row = row_ids[i];
      while (current_row < row) {
        offsets[current_row + 1] = i;
        current_row++;
      }
      current_row = row;
    }
    while (current_row < rows) {
      offsets[current_row + 1] = nnz;
      current_row++;
    }
    offsets[0] = 0;
    // The loop above might leave offsets[current_row + 1] unset if current_row == rows
    // but the loop condition current_row < rows handles it. 
    // Let's refine the logic for clarity and speed:
    
    // Refined linear scan:
    uint32_t row_ptr = 0;
    for (size_t i = 0; i < nnz; ++i) {
      uint32_t r = row_ids[i];
      while (row_ptr < r) {
        offsets[row_ptr++] = i;
      }
      row_ptr = r;
    }
    while (row_ptr <= rows) {
      offsets[row_ptr++] = nnz;
    }
    // Wait, the logic above is slightly off on indices. 
    // Standard CSR: offsets[r] is start of row r. offsets[rows] is total nnz.
    // Corrected:
    uint32_t r_idx = 0;
    for (size_t i = 0; i < nnz; ++i) {
      uint32_t r = row_ids[i];
      while (r_idx < r) {
        offsets[r_idx++] = i;
      }
      r_idx = r;
    }
    while (r_idx <= rows) {
      offsets[r_idx++] = nnz;
    }
    // The loop above sets offsets[0...rows]. 
    // But we need offsets[0...rows]. 
    // Let's use the most robust sorted-COO to CSR logic:
    
    // Reset
    for (size_t i = 0; i <= rows; ++i) offsets[i] = 0;
    for (size_t i = 0; i < nnz; ++i) {
      offsets[row_ids[i] + 1]++;
    }
    for (size_t i = 0; i < rows; ++i) {
      offsets[i + 1] += offsets[i];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}