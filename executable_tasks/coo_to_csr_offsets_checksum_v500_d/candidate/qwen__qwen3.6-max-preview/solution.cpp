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
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;
  const uint32_t nnz = static_cast<uint32_t>(row_ids.size());
  const uint32_t* ids = row_ids.data();
  uint32_t* off = offsets.data();

  for (int iter = 0; iter < iters; ++iter) {
    off[0] = 0;
    uint32_t r = 0;
    for (uint32_t idx = 0; idx < nnz; ++idx) {
      uint32_t row = ids[idx];
      while (r < row) {
        off[r + 1] = idx;
        ++r;
      }
    }
    while (r < rows) {
      off[r + 1] = nnz;
      ++r;
    }

    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i <= rows; ++i) {
      hash = mix(hash, off[i]);
    }
  }
  return hash;
}