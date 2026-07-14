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

      if (i >= nnz) {
        for (uint32_t r = current_row; r < rows; ++r) {
          offsets[static_cast<std::size_t>(r) + 1u] = static_cast<uint32_t>(nnz);
        }
        break;
      }

      const uint32_t row = row_ids[i];
      if (row >= rows) {
        for (uint32_t r = current_row; r < rows; ++r) {
          offsets[static_cast<std::size_t>(r) + 1u] = static_cast<uint32_t>(i);
        }
        break;
      }

      while (current_row < row) {
        offsets[static_cast<std::size_t>(current_row) + 1u] = static_cast<uint32_t>(i);
        ++current_row;
      }

      const std::size_t start = i;
      do {
        ++i;
      } while (i < nnz && row_ids[i] == row);

      const uint32_t end = static_cast<uint32_t>(i);
      offsets[static_cast<std::size_t>(row) + 1u] = end;
      current_row = row + 1u;
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }

  return hash;
}
