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
  // Pre-allocate offsets vector to avoid repeated allocations
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (size_t i = 0; i < offsets.size(); ++i) offsets[i] = 0;

    // Since row_ids are sorted, we can count occurrences efficiently
    // row_ids[i] is the row index of the i-th non-zero element
    // The number of elements in row r is the count of r in row_ids
    // The offset for row r+1 is the number of elements in rows 0 to r
    
    // We can simply iterate through row_ids and increment the offset for the row
    // But a more direct way to build the CSR offset array from sorted COO is:
    // offsets[r+1] = count of elements with row_id <= r
    
    size_t nnz = row_ids.size();
    if (nnz == 0) {
      // All offsets are 0
    } else {
      // The offset for row r is the index of the first element in row_ids with row_id >= r
      // Because row_ids is sorted, we can find the boundaries
      size_t current_row = 0;
      size_t count = 0;
      for (size_t i = 0; i < nnz; ++i) {
        uint32_t r = row_ids[i];
        while (current_row < r) {
          offsets[++current_row] = count;
        }
        count++;
      }
      while (current_row < rows) {
        offsets[++current_row] = count;
      }
    }
    // The last element is always the total NNZ
    offsets[rows] = static_cast<uint32_t>(nnz);

    // Calculate checksum
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}