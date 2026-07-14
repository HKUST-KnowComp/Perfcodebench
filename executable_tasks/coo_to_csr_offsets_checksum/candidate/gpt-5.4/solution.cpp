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
    offsets[0] = 0;

    std::size_t i = 0;
    uint32_t current_row = 0;

    while (current_row < rows) {
      while (i < nnz && row_ids[i] < current_row) {
        ++i;
      }

      const std::size_t start = i;
      while (i < nnz && row_ids[i] == current_row) {
        ++i;
      }

      offsets[static_cast<std::size_t>(current_row) + 1u] =
          offsets[static_cast<std::size_t>(current_row)] + static_cast<uint32_t>(i - start);
      ++current_row;
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }

  return hash;
}
