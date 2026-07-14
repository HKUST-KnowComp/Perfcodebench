#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear offsets array
    for (std::size_t i = 0; i <= rows; ++i) {
      offsets[i] = 0;
    }
    
    // Build histogram: count occurrences of each row
    // Since row_ids are sorted, this is cache-friendly
    for (uint32_t row : row_ids) {
      ++offsets[static_cast<std::size_t>(row) + 1u];
    }
    
    // Convert counts to offsets via prefix sum
    for (std::size_t i = 1; i <= rows; ++i) {
      offsets[i] += offsets[i - 1];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  
  return hash;
}
