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
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(offset_size);
  uint64_t hash = 0;
  
  const uint32_t* __restrict row_ptr = row_ids.data();
  uint32_t* __restrict cnt_ptr = counts.data();
  uint32_t* __restrict off_ptr = offsets.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts to zero
    for (size_t i = 0; i < rows; ++i) {
      cnt_ptr[i] = 0;
    }
    
    // Count occurrences of each row - simple loop for sorted or unsorted input
    for (size_t i = 0; i < n; ++i) {
      ++cnt_ptr[row_ptr[i]];
    }
    
    // Compute prefix sum to get offsets
    off_ptr[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      off_ptr[r + 1] = off_ptr[r] + cnt_ptr[r];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < offset_size; ++i) {
      hash = mix(hash, off_ptr[i]);
    }
  }
  return hash;
}