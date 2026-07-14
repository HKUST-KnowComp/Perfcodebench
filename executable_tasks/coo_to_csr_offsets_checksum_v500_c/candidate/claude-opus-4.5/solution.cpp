#include "interface.h"

#include <cstring>
#include <vector>

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
  
  const uint32_t* __restrict row_ptr = row_ids.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Fast zero using memset
    std::memset(offsets.data(), 0, offset_size * sizeof(uint32_t));
    
    // Since row_ids are sorted, we can count occurrences directly
    // by iterating and incrementing counts in a temporary array,
    // then computing prefix sums.
    // But even simpler: count each row_id occurrence
    for (size_t i = 0; i < n; ++i) {
      // offsets[row+1] will hold the count for row, then we prefix sum
      offsets[row_ptr[i] + 1]++;
    }
    
    // Prefix sum to convert counts to offsets
    for (uint32_t r = 1; r <= rows; ++r) {
      offsets[r] += offsets[r - 1];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* __restrict off_ptr = offsets.data();
    for (size_t i = 0; i <= rows; ++i) {
      hash = mix(hash, off_ptr[i]);
    }
  }
  return hash;
}