#include "interface.h"

#include <cstddef>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = row_ids.size();
  std::vector<uint32_t> offsets;
  offsets.resize(static_cast<std::size_t>(rows) + 1u);
  offsets[0] = 0u;

  std::size_t idx = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    while (idx < n && row_ids[idx] == r) {
      ++idx;
    }
    offsets[static_cast<std::size_t>(r + 1u)] = static_cast<uint32_t>(idx);
  }

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis
  for (uint32_t v : offsets) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;  // FNV-1a prime
  }
  return hash;
}
