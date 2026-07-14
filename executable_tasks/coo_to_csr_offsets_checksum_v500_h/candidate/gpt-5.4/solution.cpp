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
    uint32_t prev = 0;

    while (i < nnz) {
      const uint32_t row = row_ids[i];
      while (prev < row) {
        offsets[static_cast<std::size_t>(prev + 1u)] = static_cast<uint32_t>(i);
        ++prev;
      }
      do {
        ++i;
      } while (i < nnz && row_ids[i] == row);
      offsets[static_cast<std::size_t>(row + 1u)] = static_cast<uint32_t>(i);
      prev = row + 1u;
    }

    while (prev < rows) {
      offsets[static_cast<std::size_t>(prev + 1u)] = static_cast<uint32_t>(nnz);
      ++prev;
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }

  return hash;
}
