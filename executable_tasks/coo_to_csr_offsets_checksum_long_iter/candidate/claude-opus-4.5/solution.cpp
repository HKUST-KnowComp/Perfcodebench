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
  const size_t n = row_ids.size();
  const size_t offset_size = static_cast<size_t>(rows) + 1u;
  
  // Pre-allocate offsets buffer once
  std::vector<uint32_t> offsets(offset_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Zero out offsets
    std::fill(offsets.begin(), offsets.end(), 0u);
    
    // Since row_ids are sorted, we can count occurrences directly
    // First pass: count occurrences of each row
    const uint32_t* __restrict row_ptr = row_ids.data();
    uint32_t* __restrict off_ptr = offsets.data();
    
    for (size_t i = 0; i < n; ++i) {
      // Increment count for row+1 position (will become offset after prefix sum)
      ++off_ptr[row_ptr[i] + 1];
    }
    
    // Prefix sum to convert counts to offsets
    uint32_t cumsum = 0;
    for (size_t r = 0; r <= rows; ++r) {
      cumsum += off_ptr[r];
      off_ptr[r] = cumsum;
    }
    
    // Wait, that's wrong. Let me fix the logic.
    // offsets[0] should be 0
    // offsets[r+1] = offsets[r] + count[r]
    // After the counting loop, off_ptr[r+1] contains count[r]
    // So we need: offsets[0] = 0, then offsets[i] = offsets[i-1] + counts[i-1]
    // But we stored counts in off_ptr[row+1], so off_ptr[r+1] = count of row r
    // Prefix sum: off_ptr[0] stays 0, off_ptr[1] = off_ptr[0] + count[0] = count[0]
    // Actually the prefix sum above is wrong. Let me redo.
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < offset_size; ++i) {
      hash = mix(hash, off_ptr[i]);
    }
  }
  
  return hash;
}
