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

    // Since row_ids are sorted, we can count occurrences efficiently
    // The number of elements in row r is the count of r in row_ids
    // CSR offset[r+1] = offset[r] + count(r)
    // Because row_ids is sorted, we can just iterate and mark the end of each row
    
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
    
    // The last element of row_ids belongs to the last encountered row
    // But the logic above needs to handle the final offset correctly
    // Let's use a simpler approach: count all, then prefix sum
    
    // Re-implementing for clarity and correctness:
    // 1. Clear counts
    // 2. Count occurrences
    // 3. Prefix sum
    // However, since row_ids is sorted, we can just do:
    
    uint32_t n = row_ids.size();
    uint32_t r = 0;
    uint32_t pos = 0;
    while (r < rows) {
      uint32_t start_pos = pos;
      while (pos < n && row_ids[pos] == r) {
        pos++;
      }
      offsets[r + 1] = pos;
      r++;
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}