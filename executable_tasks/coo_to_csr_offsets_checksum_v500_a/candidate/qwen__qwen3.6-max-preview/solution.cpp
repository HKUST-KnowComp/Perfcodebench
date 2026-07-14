#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  uint64_t hash = 0;
  const uint32_t* ids = row_ids.data();
  const uint32_t n = static_cast<uint32_t>(row_ids.size());
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    uint32_t idx = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      hash ^= idx;
      hash *= 1099511628211ULL;
      while (idx < n && ids[idx] == r) {
        ++idx;
      }
    }
    hash ^= idx;
    hash *= 1099511628211ULL;
  }
  return hash;
}
