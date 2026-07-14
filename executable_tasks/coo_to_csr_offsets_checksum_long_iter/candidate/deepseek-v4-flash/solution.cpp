#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Precompute CSR offsets (size rows+1) from sorted row_ids
  std::vector<uint32_t> offsets(rows + 1, 0);
  std::size_t idx = 0;
  const std::size_t nnz = row_ids.size();
  for (uint32_t r = 0; r < rows; ++r) {
    uint32_t count = 0;
    while (idx < nnz && row_ids[idx] == r) {
      ++count;
      ++idx;
    }
    offsets[r + 1] = offsets[r] + count;
  }
  // If any trailing row_ids (>= rows) exist, they are ignored (consistent with original)
  
  const uint64_t base = 1469598103934665603ULL;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = base;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
