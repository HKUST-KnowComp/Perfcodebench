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
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t current_idx = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      while (current_idx < row_ids.size() && row_ids[current_idx] == r) {
        current_idx++;
      }
      offsets[r + 1] = current_idx;
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}