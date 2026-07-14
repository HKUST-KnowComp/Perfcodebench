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
  
  // Pre-allocate buffers outside the loop
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(offset_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear counts
    std::fill(counts.begin(), counts.end(), 0);
    
    // Count occurrences of each row - simple histogram
    // Since row_ids are sorted, we could use run-length but histogram is cache-friendly
    const uint32_t* __restrict row_ptr = row_ids.data();
    uint32_t* __restrict cnt_ptr = counts.data();
    
    for (size_t i = 0; i < n; ++i) {
      ++cnt_ptr[row_ptr[i]];
    }
    
    // Compute prefix sum to get offsets
    uint32_t* __restrict off_ptr = offsets.data();
    off_ptr[0] = 0;
    
    uint32_t cumsum = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      cumsum += cnt_ptr[r];
      off_ptr[r + 1] = cumsum;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < offset_size; ++i) {
      hash = mix(hash, off_ptr[i]);
    }
  }
  
  return hash;
}
