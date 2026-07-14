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
  const size_t n_rows = rows;
  const size_t n_ids = row_ids.size();
  
  std::vector<uint32_t> offsets(n_rows + 1);
  
  const uint32_t* ids = row_ids.data();
  uint32_t* offs = offsets.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t idx = 0;
    offs[0] = 0;
    
    for (uint32_t r = 0; r < rows; ++r) {
      uint32_t cnt = 0;
      while (idx < n_ids && ids[idx] == r) {
        ++cnt;
        ++idx;
      }
      offs[r + 1] = offs[r] + cnt;
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i <= n_rows; ++i) {
      hash = mix(hash, offs[i]);
    }
  }
  
  return hash;
}
