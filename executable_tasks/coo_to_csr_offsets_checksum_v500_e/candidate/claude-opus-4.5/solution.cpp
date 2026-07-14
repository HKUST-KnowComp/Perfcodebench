#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const size_t n = row_ids.size();
  const size_t offset_size = static_cast<size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(offset_size);
  uint64_t hash = 0;
  
  const uint32_t* __restrict__ row_ptr = row_ids.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Since row_ids are sorted, we can compute offsets directly
    // offsets[r] = number of elements with row_id < r
    // offsets[r+1] - offsets[r] = count of elements in row r
    
    std::memset(offsets.data(), 0, offset_size * sizeof(uint32_t));
    
    if (n > 0) {
      // Count elements per row using the sorted property
      // For sorted input, we just need to count how many elements are in each row
      size_t i = 0;
      while (i < n) {
        uint32_t current_row = row_ptr[i];
        size_t count = 1;
        ++i;
        while (i < n && row_ptr[i] == current_row) {
          ++count;
          ++i;
        }
        if (current_row < rows) {
          offsets[current_row + 1] = static_cast<uint32_t>(count);
        }
      }
      
      // Prefix sum to get offsets
      for (uint32_t r = 1; r <= rows; ++r) {
        offsets[r] += offsets[r - 1];
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t j = 0; j < offset_size; ++j) {
      hash = mix(hash, offsets[j]);
    }
  }
  
  return hash;
}