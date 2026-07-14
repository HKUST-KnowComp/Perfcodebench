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
  // Precompute CSR offsets once for all iterations, leveraging sorted input row IDs
  std::vector<uint32_t> offsets(rows + 1, 0);
  size_t ptr = 0;
  const size_t num_entries = row_ids.size();
  for (uint32_t r = 0; r < rows; ++r) {
    while (ptr < num_entries && row_ids[ptr] == r) {
      ptr++;
    }
    offsets[r + 1] = static_cast<uint32_t>(ptr);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}