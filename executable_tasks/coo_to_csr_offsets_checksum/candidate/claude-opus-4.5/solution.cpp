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
  
  const uint32_t* __restrict__ row_ptr = row_ids.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear offsets - use memset for speed
    std::memset(offsets.data(), 0, offset_size * sizeof(uint32_t));
    
    // Since row_ids are sorted, we can count occurrences directly
    // by incrementing counts for each row
    for (size_t i = 0; i < n; ++i) {
      ++offsets[static_cast<size_t>(row_ptr[i]) + 1u];
    }
    
    // Convert counts to offsets via prefix sum
    uint32_t cumsum = 0;
    for (size_t r = 0; r < offset_size; ++r) {
      cumsum += offsets[r];
      offsets[r] = cumsum;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* __restrict__ off_ptr = offsets.data();
    
    // Unroll hash computation
    size_t i = 0;
    const size_t unroll_end = offset_size - (offset_size % 4);
    for (; i < unroll_end; i += 4) {
      hash = mix(hash, off_ptr[i]);
      hash = mix(hash, off_ptr[i + 1]);
      hash = mix(hash, off_ptr[i + 2]);
      hash = mix(hash, off_ptr[i + 3]);
    }
    for (; i < offset_size; ++i) {
      hash = mix(hash, off_ptr[i]);
    }
  }
  
  return hash;
}