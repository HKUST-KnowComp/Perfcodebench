#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t hash = 0;
  const size_t n = row_ids.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t idx = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      while (idx < n && row_ids[idx] == r) {
        ++idx;
      }
      offsets[r + 1] = static_cast<uint32_t>(idx);
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}