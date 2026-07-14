#include "interface.h"

#include <cstring>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const size_t offsets_size = static_cast<size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(offsets_size, 0u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(offsets.data(), 0, offsets_size * sizeof(uint32_t));

    for (uint32_t row : row_ids) {
      offsets[static_cast<size_t>(row) + 1u]++;
    }

    for (uint32_t r = 0; r < rows; ++r) {
      offsets[static_cast<size_t>(r) + 1u] += offsets[r];
    }

    final_hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      final_hash = mix(final_hash, v);
    }
  }

  return final_hash;
}