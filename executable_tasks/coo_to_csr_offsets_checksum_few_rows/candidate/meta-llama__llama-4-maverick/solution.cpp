#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    offsets.assign(static_cast<std::size_t>(rows) + 1u, 0);
    for (uint32_t row : row_ids) {
      ++offsets[static_cast<std::size_t>(row) + 1u];
    }
    for (uint32_t r = 1; r <= rows; ++r) {
      offsets[static_cast<std::size_t>(r)] += offsets[static_cast<std::size_t>(r - 1)];
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
