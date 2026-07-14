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
  std::size_t n = row_ids.size();
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t idx = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r] = static_cast<uint32_t>(idx);
      while (idx < n && row_ids[idx] == r) {
        ++idx;
      }
    }
    offsets[rows] = static_cast<uint32_t>(idx);

    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i <= rows; ++i) {
      hash = mix(hash, offsets[i]);
    }
    final_hash = hash;
  }
  return final_hash;
}
