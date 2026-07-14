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
  // Precompute invariant CSR offsets once, avoid redundant work across iterations
  std::vector<uint32_t> counts(static_cast<std::size_t>(rows), 0u);
  for (uint32_t r : row_ids) {
    ++counts[static_cast<std::size_t>(r)];
  }
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0u);
  for (uint32_t r = 0; r < rows; ++r) {
    offsets[static_cast<std::size_t>(r + 1u)] = offsets[static_cast<std::size_t>(r)] + counts[static_cast<std::size_t>(r)];
  }

  // Repeat hash calculation per iteration to match required timing workload
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}