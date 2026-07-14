#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  return (hash ^ value) * 1099511628211ULL;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
  const size_t nnz = row_ids.size();
  const uint32_t* rid = row_ids.data();

  // Streamed construction of CSR offsets and hash without materializing the array.
  uint64_t hash = 1469598103934665603ULL;
  // offsets[0] = 0
  hash = mix(hash, 0u);

  size_t j = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    while (j < nnz && rid[j] == r) {
      ++j;
    }
    // offsets[r + 1] = j
    hash = mix(hash, static_cast<uint32_t>(j));
  }

  return hash;
}
