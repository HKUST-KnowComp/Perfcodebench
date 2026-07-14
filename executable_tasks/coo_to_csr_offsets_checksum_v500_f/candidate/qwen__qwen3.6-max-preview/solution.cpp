#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0);
    
    for (uint32_t r : row_ids) {
      offsets[r + 1]++;
    }
    
    uint32_t acc = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      acc += offsets[r + 1];
      offsets[r + 1] = acc;
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}