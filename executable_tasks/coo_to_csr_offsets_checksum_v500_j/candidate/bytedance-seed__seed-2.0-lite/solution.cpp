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
  const size_t nnz = row_ids.size();
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t last_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    size_t ptr = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      while (ptr < nnz && row_ids[ptr] == r) {
        ++ptr;
      }
      offsets[r + 1] = static_cast<uint32_t>(ptr);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    last_hash = hash;
  }

  return last_hash;
}