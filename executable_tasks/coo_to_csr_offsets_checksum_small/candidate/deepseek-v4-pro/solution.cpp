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
  // Build CSR offsets once from the sorted row_ids
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  offsets[0] = 0;
  size_t pos = 0;
  const size_t n = row_ids.size();

  for (uint32_t r = 0; r < rows; ++r) {
    uint32_t count = 0;
    while (pos < n && row_ids[pos] == r) {
      ++count;
      ++pos;
    }
    offsets[r + 1] = offsets[r] + count;
  }

  // Compute the hash for the required number of iterations
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
