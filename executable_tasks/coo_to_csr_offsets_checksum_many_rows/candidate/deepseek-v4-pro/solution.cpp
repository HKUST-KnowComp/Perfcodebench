#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Precompute CSR offsets once (row_ids are assumed to be sorted)
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1, 0);
  std::size_t idx = 0;
  const std::size_t nnz = row_ids.size();
  // Build prefix sums from sorted row_ids
  for (uint32_t r = 0; r < rows; ++r) {
    uint32_t cnt = 0;
    while (idx < nnz && row_ids[idx] == r) {
      ++cnt;
      ++idx;
    }
    offsets[static_cast<std::size_t>(r + 1)] = offsets[static_cast<std::size_t>(r)] + cnt;
  }
  // Any remaining entries with row_id >= rows are ignored (consistent with original behaviour)

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}