#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0);
    
    for (uint32_t r : row_ids) {
      offsets[r + 1]++;
    }
    
    for (uint32_t i = 1; i <= rows; ++i) {
      offsets[i] += offsets[i - 1];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}