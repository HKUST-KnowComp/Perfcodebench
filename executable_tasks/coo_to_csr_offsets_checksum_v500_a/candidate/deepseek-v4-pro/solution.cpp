#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Build CSR offsets once
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0);
  std::size_t idx = 0;
  const std::size_t n = row_ids.size();
  for (uint32_t r = 0; r < rows; ++r) {
    std::size_t start = idx;
    while (idx < n && row_ids[idx] == r) {
      ++idx;
    }
    uint32_t cnt = static_cast<uint32_t>(idx - start);
    offsets[static_cast<std::size_t>(r) + 1u] =
        offsets[static_cast<std::size_t>(r)] + cnt;
  }
  // The remaining rows (if idx exhausted) have zero count, already set by initialization

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}