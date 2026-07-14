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
  const size_t offsets_size = static_cast<size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(offsets_size);
  const size_t nnz = row_ids.size();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    size_t ptr = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      while (ptr < nnz && row_ids[ptr] == r) {
        ++ptr;
      }
      offsets[static_cast<size_t>(r + 1u)] = static_cast<uint32_t>(ptr);
    }
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}