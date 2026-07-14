#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Count occurrences of each row
    std::memset(counts.data(), 0, rows * sizeof(uint32_t));
    for (uint32_t row : row_ids) {
      ++counts[row];
    }
    
    // Build offsets via prefix sum
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[static_cast<std::size_t>(r + 1u)] = offsets[r] + counts[r];
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
