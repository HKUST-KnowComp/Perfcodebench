#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0);
    
    for (uint32_t row : row_ids) {
      offsets[row]++;
    }
    
    uint32_t current = 0;
    hash = 1469598103934665603ULL;
    for (uint32_t r = 0; r <= rows; ++r) {
      uint32_t val = offsets[r];
      offsets[r] = current;
      hash ^= static_cast<uint64_t>(current);
      hash *= 1099511628211ULL;
      current += val;
    }
  }
  return hash;
}