#include "interface.h"

#include <vector>

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
    // Zero out offsets array
    for (std::size_t i = 0; i <= rows; ++i) {
      offsets[i] = 0;
    }
    
    // Build histogram directly from sorted row_ids
    // offsets[r+1] will hold count of row r
    for (uint32_t row : row_ids) {
      ++offsets[static_cast<std::size_t>(row) + 1u];
    }
    
    // Convert counts to prefix sum (CSR offsets)
    for (uint32_t r = 1; r <= rows; ++r) {
      offsets[r] += offsets[r - 1];
    }
    
    // Compute hash with loop unrolling
    hash = 1469598103934665603ULL;
    std::size_t i = 0;
    std::size_t limit = offsets.size();
    std::size_t unroll_limit = limit - (limit % 4);
    
    for (; i < unroll_limit; i += 4) {
      hash = mix(hash, offsets[i]);
      hash = mix(hash, offsets[i + 1]);
      hash = mix(hash, offsets[i + 2]);
      hash = mix(hash, offsets[i + 3]);
    }
    
    for (; i < limit; ++i) {
      hash = mix(hash, offsets[i]);
    }
  }
  
  return hash;
}
