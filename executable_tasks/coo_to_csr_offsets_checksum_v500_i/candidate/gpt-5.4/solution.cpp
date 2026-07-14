#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const std::size_t nnz = row_ids.size();
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t prev_row = 0;
    std::size_t idx = 0;
    offsets[0] = 0;

    while (idx < nnz) {
      const uint32_t r = row_ids[idx];
      while (prev_row < r) {
        offsets[static_cast<std::size_t>(prev_row) + 1u] = static_cast<uint32_t>(idx);
        ++prev_row;
      }
      do {
        ++idx;
      } while (idx < nnz && row_ids[idx] == r);
      offsets[static_cast<std::size_t>(r) + 1u] = static_cast<uint32_t>(idx);
      prev_row = r + 1u;
    }

    while (prev_row < rows) {
      offsets[static_cast<std::size_t>(prev_row) + 1u] = static_cast<uint32_t>(nnz);
      ++prev_row;
    }

    hash = 1469598103934665603ULL;
    for (std::size_t i = 0, n = static_cast<std::size_t>(rows) + 1u; i < n; ++i) {
      hash = mix(hash, offsets[i]);
    }
  }

  return hash;
}
