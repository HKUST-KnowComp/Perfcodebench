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
  uint64_t final_hash = 0;
  const size_t nnz = row_ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    uint32_t current_offset = 0;
    uint32_t current_row = 0;
    size_t i = 0;

    // The offsets array has size rows + 1
    // offsets[0] is always 0
    hash = mix(hash, 0);

    while (current_row < rows) {
      // Count how many elements belong to current_row
      while (i < nnz && row_ids[i] == current_row) {
        i++;
      }
      
      // The offset for the next row is the current offset + count of current row
      current_offset = static_cast<uint32_t>(i);
      hash = mix(hash, current_offset);
      current_row++;
    }
    
    // If there are rows at the end with no elements, they all share the same final offset
    // However, the loop above handles current_row < rows. 
    // If nnz was reached early, the remaining offsets are all equal to nnz.
    // The loop logic above already handles this because if i == nnz, 
    // the inner while is skipped and current_offset remains nnz.
    
    final_hash = hash;
  }
  return final_hash;
}